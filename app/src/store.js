import {applyMiddleware, createStore} from 'redux';

import { createLogger } from 'redux-logger';
import { createPromise } from 'redux-promise-middleware';

import reducer from './reducers/reducers';

import { default as thunk} from 'redux-thunk';

const middleware = applyMiddleware(createPromise(), thunk, createLogger());

console.log(middleware);
console.log(thunk);

export default createStore(reducer, middleware);