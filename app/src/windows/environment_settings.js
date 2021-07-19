const electron = require('electron')
const { ipcMain } = require('electron')
const BrowserWindow = electron.BrowserWindow

let envWindow = undefined;
let isVisible = false;
let forceQuit = false;

module.exports = function createEnvironmentWindow(path, parentCenter, server)
{
    const w = 800;
    const h = 600;

	if (isVisible === false)
	{
		isVisible = true;
		forceQuit = false;

		envWindow = new BrowserWindow({
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
		
		envWindow.removeMenu();		
		//envWindow.webContents.openDevTools();
		envWindow.loadURL(path);

		envWindow.webContents.on('did-finish-load', e => 
		{
			envWindow.webContents.send('loadEnvironment', server);
		});

		envWindow.on('close', (e) => 
		{
			try
			{
				if (!forceQuit) 
				{
					envWindow.webContents.send('closeIssued');
					e.preventDefault();
				}
				else
					isVisible = false;	
			}
			catch(e)
			{

			}
		});

		ipcMain.on('closeEnvWindow', (event, arg) => 
		{
			try
			{
				forceQuit = true;
				envWindow.close();
			}
			catch(e)
			{

			}
		});
	}
	else
		envWindow.focus();
}