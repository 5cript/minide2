const {combineReducers} = require('redux');

const openFiles = require('./open_files');
const workspace = require('./workspace');
const backend = require('./backend');
const shortcuts = require('./shortcuts');
const toolbars = require('./toolbars');
const locale = require('./locale');
const logs = require('./logs');
const preferences = require('./preferences');
const debugging = require('./debugging_reducer');
const misc = require('./miscellaneous');

module.exports = combineReducers({
    openFiles,
    workspace,
    backend,
    shortcuts,
    toolbars,
    locale,
    logs,
    preferences,
    debugging,
    misc
})