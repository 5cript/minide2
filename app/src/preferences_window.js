const electron = require('electron')
const { ipcMain } = require('electron')
const BrowserWindow = electron.BrowserWindow

let preferencesWindow = undefined;
let isVisible = false;
let forceQuit = false;

export default function createPreferencesWindow(path, parentCenter, server)
{
    const w = 1400;
    const h = 950;

	if (isVisible === false)
	{
		isVisible = true;
		forceQuit = false;

		preferencesWindow = new BrowserWindow({
			x: parentCenter.x - w / 2,
			y: parentCenter.y - h / 2,
			width: w,
			height: h,
			webPreferences: {
				webSecurity: true,
				nodeIntegration: true,
				allowEval: false
			}
		})
		
		preferencesWindow.removeMenu();		
		preferencesWindow.webContents.openDevTools();
		preferencesWindow.loadURL(path);

        /*
		preferencesWindow.webContents.on('did-finish-load', e => 
		{
			preferencesWindow.webContents.send('loadEnvironment', server);
        });
        */

		preferencesWindow.on('close', (e) => 
		{
			try
			{
				if (!forceQuit) 
				{
					preferencesWindow.webContents.send('closeIssued');
					e.preventDefault();
				}
				else
					isVisible = false;	
			}
			catch(e)
			{

			}
		});

		ipcMain.on('closePrefWindow', (event, arg) => 
		{
			try
			{
				forceQuit = true;
				preferencesWindow.close();
			}
			catch(e)
			{

			}
		});
	}
	else
		envWindow.focus();
}