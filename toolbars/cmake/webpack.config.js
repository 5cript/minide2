const webpack = require('webpack');
const fs = require('fs');
const path = require('path');

const compiler = webpack({

});

const appData = process.env.APPDATA || (process.platform == 'darwin' ? process.env.HOME + '/Library/Preferences' : process.env.HOME + "/.local/share");

compiler.run((err, stats) => {

    compiler.close((closeErr) => {
        fs.copyFileSync('./dist/main.js', path.join(appData, '.minIDE', 'toolbars', 'cmake', 'main.js'));
    });
});