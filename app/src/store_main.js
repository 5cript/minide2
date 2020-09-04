const {createStore, applyMiddleware, compose} = require('redux');
const { forwardToRenderer, triggerAlias, replayActionMain } = require('electron-redux');

const reducer = require('./reducers/reducers');

let store = createStore(
    reducer,
    applyMiddleware(
        // <---- other middlewares HERE
        forwardToRenderer
    )
);

replayActionMain(store);

module.exports = store;