const webpack = require('webpack');
const fs = require('fs');
const path = require('path');

const compiler = webpack({});

const appData = process.env.APPDATA || (process.platform == 'darwin' ? process.env.HOME + '/Library/Preferences' : process.env.HOME + "/.local/share");

compiler.run((err, stats) => {
    const deployTarget = path.join(appData, '.minIDE', 'plugins', 'cmake', 'main.js');
    if (fs.existsSync(deployTarget))
        fs.unlinkSync(deployTarget);
    fs.copyFileSync('./dist/main.js', deployTarget);

    compiler.close((closeErr) => {
    });
});

module.exports = {
    externals: {
        'minide_plugin': 'minide_plugin'
    }
}