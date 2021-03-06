const CopyWebpackPlugin = require('copy-webpack-plugin');
const MonacoWebpackPlugin = require('monaco-editor-webpack-plugin');

//import {CopyWebpackPlugin} from 'copy-webpack-plugin';
//import {MonacoWebpackPlugin} from 'monaco-editor-webpack-plugin';

module.exports = function override(config, env) {
    if (!config.plugins) {
        config.plugins = [];
    }

    config.plugins.push(new MonacoWebpackPlugin({
      /*languages: ['json']*/
    }));

	/*
    config.plugins.push(
        (process.env.NODE_ENV === 'production') ?
        new CopyWebpackPlugin([{from: "node_modules/monaco-editor/min/vs/", to: "vs"}]) :
        new CopyWebpackPlugin([{from: "node_modules/monaco-editor/min/vs/", to: "dist/vs"}])
    );
	*/

    return config;
}