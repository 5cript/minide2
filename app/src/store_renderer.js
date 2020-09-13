import {createStore, applyMiddleware} from 'redux';
import { forwardToMain, replayActionRenderer, getInitialStateRenderer } from 'electron-redux';

import reducer from './reducers/reducers';
import thunk from 'redux-thunk';
import promiseMiddleware from 'redux-promise-middleware';
const initialState = getInitialStateRenderer();

const store = createStore(
    reducer,
    initialState,
    applyMiddleware(
        forwardToMain,
        thunk,
        promiseMiddleware
    )
);

replayActionRenderer(store);

export default store;