const webpack = require('webpack');
const MonacoWebpackPlugin = require('monaco-editor-webpack-plugin');

module.exports = {
    reactScriptsVersion: "react-scripts" /* (default value) */,
    webpack: {
        plugins: {
            add: [
                new MonacoWebpackPlugin({}),
                new webpack.ExternalsPlugin('commonjs', ['electron'])
            ]
        }
    }
};