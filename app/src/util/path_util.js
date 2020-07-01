import nodePath from 'path';

const isRenderer = require('is-electron-renderer')

const pathModifier =
{
    shorten: (path) => {
        return nodePath.basename(path);
    },

    concat: (base, relative) => {
        return nodePath.join(base, relative);
    }
}

const minIdeHome = (fs) => 
{
    let home = process.env.APPDATA || (process.platform == 'darwin' ? process.env.HOME + '/Library/Preferences' : process.env.HOME + "/.local/share");
    home = home + "/.minIDE";
    if (!fs.existsSync(home))
        fs.mkdirSync(home, {recursive: true});
    return home;
}

export {pathModifier, minIdeHome};