const electron = require('electron')
const { ipcMain } = require('electron')
const BrowserWindow = electron.BrowserWindow

let keybindsWindow = undefined;
let isVisible = false;

module.exports = function createKeybindsWindow(path, parentCenter, server, home)
{
    const w = 1200;
    const h = 800;

	if (isVisible === false)
	{
		isVisible = true;
		forceQuit = false;

		keybindsWindow = new BrowserWindow({
			x: parentCenter.x - w / 2,
			y: parentCenter.y - h / 2,
			width: w,
			height: h,
			webPreferences: {
				webSecurity: true,
				nodeIntegration: true,
				contextIsolation: false,
				allowEval: false,
				enableRemoteModule: true
			}
		})
		
		keybindsWindow.removeMenu();		
		keybindsWindow.webContents.openDevTools();
        keybindsWindow.loadURL(path);
        
		keybindsWindow.on('close', (e) => 
		{
			isVisible = false;	
        });
        
        keybindsWindow.webContents.on('did-finish-load', e => 
		{
			keybindsWindow.webContents.send('setHome', home);
		});
	}
	else
		keybindsWindow.focus();
}