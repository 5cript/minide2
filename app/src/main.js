const electron = require('electron')
const app = electron.app
const path = require('path')
const isDev = require('electron-is-dev')
const shortcut = require('electron-localshortcut');
const BrowserWindow = electron.BrowserWindow

require('electron-reload')

process.env['ELECTRON_DISABLE_SECURITY_WARNINGS'] = 'true';

dict = require('./util/localization.js');

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
	}
]

function registerShortcuts(mainWindow) {
	shortcut.register(mainWindow, 'F12', () => {
		mainWindow.webContents.toggleDevTools();
	});
}

function createWindow() {
	menu = new electron.Menu.buildFromTemplate(menuTemplate);

	electron.Menu.setApplicationMenu(menu);

	let windowWidth = 1500;
	let windowHeight = 900;
	
	let x = undefined;
	let y = undefined;
	if (isDev) {
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
			webSecurity: false,
			nodeIntegration: true,
			allowEval: false
		},
	})

	mainWindow.loadURL(
		isDev
			? 'http://localhost:3000'
			: `file://${path.join(__dirname, '../build/index.html')}`,
	)

	if (isDev)
		mainWindow.webContents.openDevTools()

	mainWindow.on('closed', () => {
		mainWindow = null
	})

	registerShortcuts(mainWindow);
}

app.on('ready', createWindow)

app.on('window-all-closed', () => {
	if (process.platform !== 'darwin') {
		app.quit()
	}
})

app.on('activate', () => {
	if (mainWindow === null) {
		createWindow()
	}
})
