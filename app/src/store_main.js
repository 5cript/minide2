const {createStore} = require('redux');
const eduxPreload = require('electron-redux/preload');
const edux = require('electron-redux/main');

eduxPreload.preload();
const reducer = require('./reducers/reducers');

let store = createStore(
    reducer,
    edux.stateSyncEnhancer()
);

module.exports = store;