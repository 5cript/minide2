import webpack from 'webpack';
import fs from 'fs';
import path from 'path';
import { dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));

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

export default {
    externals: {
        minide: 'minide'
    },
    entry: './src/index.js',
    target: 'es2020',
    module: {
        rules: [
            {
                test: /\.tsx?$/,
                use: 'node-loader',
                exclude: /node_modules/,
            }
        ]
    },
    resolve: {
        extensions: ['.js', '.jsx'],
    },
    output: {
        filename: 'main.js',
        path: path.resolve(__dirname, "./dist"),
        environment: {module: true},
        library: {
            type: 'module'
        }
    },
    externalsType: 'module',
    devtool: 'source-map',
    experiments: {
        outputModule: true
    }
}