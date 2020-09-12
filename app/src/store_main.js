const {createStore, applyMiddleware, compose} = require('redux');
const { forwardToRenderer, triggerAlias, replayActionMain } = require('electron-redux');

const reducer = require('./reducers/reducers');
const thunk = require('redux-thunk').default;
const promiseMiddleware = require('redux-promise-middleware').default;

const middleware =  [
    thunk,
    promiseMiddleware,
    forwardToRenderer
]

const enhanced = [
    applyMiddleware(...middleware)
]

const enhancer = compose(...enhanced)

let store = createStore(
    reducer,
    enhancer
);

replayActionMain(store);

module.exports = store;