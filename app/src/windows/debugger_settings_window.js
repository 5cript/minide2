const electron = require('electron')
const { ipcMain } = require('electron')
const BrowserWindow = electron.BrowserWindow

let debuggerSettingsWindow = undefined;
let isVisible = false;

module.exports = function createDebuggerSettingsWindow(path, parentCenter, home)
{
    const w = 1600;
    const h = 800;

	if (isVisible === false)
	{
		isVisible = true;
		forceQuit = false;

		debuggerSettingsWindow = new BrowserWindow({
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
		
		debuggerSettingsWindow.removeMenu();		
		debuggerSettingsWindow.webContents.openDevTools();
        debuggerSettingsWindow.loadURL(path);
        
		debuggerSettingsWindow.on('close', (e) => 
		{
			isVisible = false;	
        });
        
        debuggerSettingsWindow.webContents.on('did-finish-load', e => 
		{
			debuggerSettingsWindow.webContents.send('setHome', home);
		});
	}
	else
		debuggerSettingsWindow.focus();
}