const CopyWebpackPlugin = require('copy-webpack-plugin');

module.exports = function override(config, env) {
    if (!config.plugins) {
        config.plugins = [];
    }

    config.plugins.push(
        (process.env.NODE_ENV === 'production') ?
        new CopyWebpackPlugin([{from: "node_modules/monaco-editor/min/vs/", to: "vs"}]) :
        new CopyWebpackPlugin([{from: "node_modules/monaco-editor/min/vs/", to: "dist/vs"}])
    );

    return config;
}