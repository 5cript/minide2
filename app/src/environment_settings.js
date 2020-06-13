const electron = require('electron')
const BrowserWindow = electron.BrowserWindow

export default function createEnvironmentWindow(path, parentCenter)
{
    const w = 800;
    const h = 600;

    const envWindow = new BrowserWindow({
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
    
	envWindow.removeMenu();
	
	envWindow.webContents.openDevTools()

	envWindow.loadURL(path);
}