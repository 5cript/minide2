let nodePath;
if (typeof window !== 'undefined') 
    nodePath = window.require('path');
else
    nodePath = require('path');

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
    let home = process.env.APPDATA || (process.platform === 'darwin' ? process.env.HOME + '/Library/Preferences' : process.env.HOME + "/.local/share");
    home = home + "/.minIDE";
    if (!fs.existsSync(home))
        fs.mkdirSync(home, {recursive: true});
    return home;
}

module.exports = {pathModifier, minIdeHome};