const {createStore} = require('redux');
//const {stateSyncEnhancer} = require('electron-redux/main');
const eduxPreload = require('electron-redux/preload');
const edux = require('electron-redux/main');
console.log(edux)

eduxPreload.preload();
const reducer = require('./reducers/reducers');

let store = createStore(
    reducer,
    edux.stateSyncEnhancer()
);

module.exports = store;