//const CopyWebpackPlugin = require('copy-webpack-plugin');
const MonacoWebpackPlugin = require('monaco-editor-webpack-plugin');

//import {CopyWebpackPlugin} from 'copy-webpack-plugin';
//import {MonacoWebpackPlugin} from 'monaco-editor-webpack-plugin';

module.exports = {
    reactScriptsVersion: "react-scripts" /* (default value) */,
    webpack: {
        plugins: {
            add: [new MonacoWebpackPlugin({})]
        }
    }
};