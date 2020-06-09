const electron = require('electron')
const app = electron.app
const path = require('path')
const isDev = require('electron-is-dev')
const shortcut = require('electron-localshortcut');
const BrowserWindow = electron.BrowserWindow

require('electron-reload')
const { ipcMain } = require('electron')

process.env['ELECTRON_DISABLE_SECURITY_WARNINGS'] = 'true';

import Dictionary from './util/localization.js';
let dict = new Dictionary();

let mainWindow;

let forceQuit = false;

const menuTemplate = [
	{
		label: dict.translate('$File', 'menu'),
		submenu: [
			{
				label: dict.translate('$OpenWorkspace', 'menu'),
				click: async e => {
					let folder = await electron.dialog.showOpenDialog(mainWindow, {properties: ['openDirectory']});
					mainWindow.webContents.send('openWorkspace', folder);
				}
			}
		]
	},
	{
		label: dict.translate('$Backend', 'menu'),
		submenu: [
			{
				label: dict.translate('$Connect', 'menu'),
				click: async e => {
					mainWindow.webContents.send('connectBackend');
				}
			},
			{
				label: dict.translate('$Test', 'menu'),
				click: async e => {
					mainWindow.webContents.send('testBackend');
				}
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
	console.log(__dirname);

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

	let port = '3000';
	//let port = '5000';
	mainWindow.loadURL(
		isDev
			? 'http://localhost:' + port
			: `file://${__dirname}/../public/index.html`
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
