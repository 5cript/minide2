import {createStore} from 'redux';

/*
import { applyMiddleware } from 'redux';
import { createLogger } from 'redux-logger';
import { createPromise } from 'redux-promise-middleware';
import { default as thunk} from 'redux-thunk';

const middleware = applyMiddleware(createPromise(), thunk, createLogger());
*/
import reducer from './reducers/reducers';

const middleware = {};

export default createStore(reducer, middleware);