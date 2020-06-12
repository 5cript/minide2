const electron = require('electron')
const app = electron.app
const path = require('path')
const isDev = require('electron-is-dev')
const shortcut = require('electron-localshortcut');
const BrowserWindow = electron.BrowserWindow

require('electron-reload')
const { ipcMain } = require('electron')

process.env['ELECTRON_DISABLE_SECURITY_WARNINGS'] = 'true';

import createEnvironmentWindow from './environment_settings';

import Dictionary from './util/localization.js';
let dict = new Dictionary();

let mainWindow;

let forceQuit = false;
let port = '3000';

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
				label: dict.translate('$Environment', 'menu'),
				click: async e => 
				{
					const path = isDev
						? `http://localhost:${port}?environments`
						: `file://${__dirname}/../public/index.html?environments`
					
					const pos = mainWindow.getPosition();
					const size = mainWindow.getSize();
					createEnvironmentWindow(path, {
						x: pos[0] + size[0] / 2,
						y: pos[1] + size[1] / 2
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
				label: dict.translate('$Test', 'menu'),
				click: async e => mainWindow.webContents.send('testBackend')
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

	let windowWidth = 1500;
	let windowHeight = 900;
	
	let x = undefined;
	let y = undefined;
	if (isDev)
	{
		let bounds = electron.screen.getAllDisplays()[1].bounds;
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
			allowEval: false
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
		if (!forceQuit) 
		{
			mainWindow.webContents.send('closeIssued');
			e.preventDefault();
		}
	});

	ipcMain.on('closeNow', (event, arg) => 
	{
		forceQuit = true;
		mainWindow.close();
	});

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
