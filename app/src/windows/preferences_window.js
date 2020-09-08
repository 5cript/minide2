const electron = require('electron');
const {ipcMain} = require('electron');
const {minIdeHome} = require('../util/path_util');
const fs = require('fs');
const BrowserWindow = electron.BrowserWindow;

let preferencesWindow = undefined;
let isVisible = false;
let forceQuit = false;

module.exports = function createPreferencesWindow(path, parentCenter)
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
				allowEval: false,
				enableRemoteModule: true
			}
		})
		
		preferencesWindow.removeMenu();		
		preferencesWindow.webContents.openDevTools();
		preferencesWindow.loadURL(path);

		preferencesWindow.webContents.on('did-finish-load', e => 
		{
            const home = minIdeHome(require('fs'));
            const preferences = fs.readFileSync(home + "/preferences.json", 'utf8');
			preferencesWindow.webContents.send('preferences', preferences);
        });

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
        
        ipcMain.on('applicationPreferencesSaved', (event, preferences) => 
        {
            const home = minIdeHome(require('fs'));
            fs.writeFileSync(home + "/preferences.json", JSON.stringify(preferences, null, 4));

            event.returnValue = '';
        })
	}
	else
		envWindow.focus();
}