const createEnvironmentWindow  = require('./environment_settings');
const createPreferencesWindow = require('./preferences_window');
const createKeybindsWindow = require('./keybinds_window');
const createDebuggerSettingsWindow = require('./debugger_settings_window');
const Dictionary = require('../util/localization.js');

const electron = require('electron')
const isDev = require('electron-is-dev')
const shortcut = require('electron-localshortcut');
const {ipcMain} = require('electron')
const {minIdeHome} = require('../util/path_util');
const fs = require('fs');
require('electron-reload')
const store = require('../store_main');

const BrowserWindow = electron.BrowserWindow;

const app = electron.app
process.env['ELECTRON_DISABLE_SECURITY_WARNINGS'] = 'true';
const dict = new Dictionary();

let mainWindow;

let forceQuit = false;
let port = '3000';
let server = {
	ip: '[::1]',
	port: 43255,
	controlPort: 43256,
	dataPort: 43256,
	authCookie: ''
};
let fastClickExitTimes = [];

const createSideWindow = ({factory, uriPart, args}) => 
{
	const path = isDev
		? `http://localhost:${port}?${uriPart}`
		: `file://${__dirname}/../public/index.html${uriPart}`
	
	const pos = mainWindow.getPosition();
	const size = mainWindow.getSize();
	factory(path, {
		x: pos[0] + size[0] / 2,
		y: pos[1] + size[1] / 2
	}, ...args);
}

const menuTemplate = [
	{
		label: dict.translate('$File', 'menu'),
		submenu: [
			{
				label: dict.translate('$OpenWorkspace', 'menu'),
				click: async e => 
				{
					let folder = await electron.dialog.showOpenDialog(mainWindow, {properties: ['openDirectory']});
					mainWindow.webContents.send('openWorkspace', folder);
				}
			}
		]
	},
	{
		label: dict.translate('$Settings', 'menu'),
		submenu: [
			{
				label: dict.translate('$Preferences', 'menu'),
				click: async e => 
				{
					const path = isDev
						? `http://localhost:${port}?preferences`
						: `file://${__dirname}/../public/index.html?preferences`
					
					const pos = mainWindow.getPosition();
					const size = mainWindow.getSize();
					createPreferencesWindow(path, {
						x: pos[0] + size[0] / 2,
						y: pos[1] + size[1] / 2
					}, server);
				}
			},
			{
				label: dict.translate('$Environment', 'menu'),
				click: async e => 
				{
					createSideWindow({
						factory: createEnvironmentWindow,
						uriPart: 'environments',
						args: [server]
					});
				}
			},
			{
				label: dict.translate('$EditKeybinds', 'menu'),
				click: async e => 
				{
					createSideWindow({
						factory: createKeybindsWindow, 
						uriPart: 'keybinds', 
						args: [server, mainWindow.home]
					});
				}
			},
			{
				label: dict.translate('$DebuggerSettings', 'menu'),
				click: async e => 
				{
					createSideWindow({
						factory: createDebuggerSettingsWindow, 
						uriPart: 'debuggerSettings', 
						args: [mainWindow.home]
					});
				}
			}
		]
	},
	{
		label: dict.translate('$Backend', 'menu'),
		submenu: [
			{
				label: dict.translate('$Connect', 'menu'),
				click: async e => mainWindow.webContents.send('connectBackend')
			},
			{
				label: dict.translate('$ReloadToolbar', 'menu'),
				click: async e => mainWindow.webContents.send('reloadToolbar')
			}
		]
	},
	{
		label: dict.translate("$Test", 'menu'),
		submenu: [
			{
				label: dict.translate('$DumpMainStore', 'menu'),
				click: async e => console.log(store.getState())
			}
		]
	}
]

function registerShortcuts(mainWindow) 
{
	shortcut.register(mainWindow, 'F12', () => 
	{
		mainWindow.webContents.toggleDevTools();
	});
}

function createWindow() 
{
	let menu = new electron.Menu.buildFromTemplate(menuTemplate);

	electron.Menu.setApplicationMenu(menu);

	//const screen = 0;
	const screen = 1;

	let windowWidth = 1500;
	let windowHeight = 900;
	
	let x = undefined;
	let y = undefined;
	if (isDev)
	{
		let bounds = electron.screen.getAllDisplays()[screen].bounds;
		x = bounds.x + bounds.width / 2 - windowWidth / 2;
		y = bounds.y + bounds.height / 2 - windowHeight / 2;
	}

	mainWindow = new BrowserWindow({
		x: x,
		y: y,
		width: windowWidth,
		height: windowHeight,
		webPreferences: {
			webSecurity: true,
			nodeIntegration: true,
			allowEval: false,
			contextIsolation: false,
			enableRemoteModule: true
		},
	})

	mainWindow.loadURL(
		isDev
			? `http://localhost:${port}?main`
			: `file://${__dirname}/../public/index.html?main`
	);

	if (isDev)
		mainWindow.webContents.openDevTools()

	mainWindow.on('closed', () => 
	{
		mainWindow = null
	})

	mainWindow.on('close', (e) => 
	{
		fastClickExitTimes.push(new Date());
		if (fastClickExitTimes.length == 3)
		{
			diff = 
				((fastClickExitTimes[2] - fastClickExitTimes[1]) +
				(fastClickExitTimes[1] - fastClickExitTimes[0])) / 2
			;
			if (Math.abs(diff) < 250)
			{
				// close anyway on fast clicks.
				return;
			}
			fastClickExitTimes.shift();
		}
		if (!forceQuit) 
		{
			mainWindow.webContents.send('closeIssued');
			e.preventDefault();
		}
	});
	
	mainWindow.webContents.on('did-finish-load', e => 
	{
		const home = minIdeHome(fs);
		let preferences;
		if (fs.existsSync(home + "/preferences.json"))
			preferences = JSON.parse(fs.readFileSync(home + "/preferences.json", 'utf8')).preferences;

		console.log('preferences loaded', preferences);

		mainWindow.home = home;

		mainWindow.webContents.send('setHome', home);
		mainWindow.webContents.send('preferences', preferences);
		mainWindow.webContents.send('setBackend', {
			ip: preferences.backend.host,
			port: preferences.backend.port,
			controlPort: preferences.backend.controlPort,
			dataPort: preferences.backend.dataPort,
			autoConnect: preferences.backend.autoConnect
		});
	});

	ipcMain.on('closeNow', (event, arg) => 
	{
		forceQuit = true;
		mainWindow.close();
	});

	/*
	REMOVE ME AT SOME POINT, if truely not needed anymore. cookie trick.

	ipcMain.on('haveCookieUpdate', (event, arg) => 
	{
		// Query all cookies.
		electron.session.defaultSession.cookies.get({})
		.then((cookies) => {
			console.log(cookies)
			const authCookieIndex = cookies.findIndex(cookie => cookie.name === 'aSID');
			if (authCookieIndex === -1)
				return;
			
			console.log(authCookieIndex)
			server.authCookie = cookies[authCookieIndex];
			server.sessionId =  server.authCookie.value;
			mainWindow.webContents.send('cookie', {
				name: server.authCookie.name,
				value: server.authCookie.value
			})
		}).catch((error) => {
			console.log(error)
		})

		event.returnValue = 0;
	});
	*/

	registerShortcuts(mainWindow);
}

app.on('ready', createWindow)

app.on('window-all-closed', () => 
{
	if (process.platform !== 'darwin') 
		app.quit()
})

app.on('activate', () => 
{
	if (mainWindow === null) 
	{
		createWindow()
	}
})
