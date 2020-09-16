import {createStore, applyMiddleware} from 'redux';
import { forwardToMain, replayActionRenderer, getInitialStateRenderer } from 'electron-redux';

import reducer from './reducers/reducers';
const initialState = getInitialStateRenderer();

const store = createStore(
    reducer,
    initialState,
    applyMiddleware(
        forwardToMain
    )
);

replayActionRenderer(store);

export default store;