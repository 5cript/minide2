import {createStore} from 'redux';
import {stateSyncEnhancer, composeWithStateSync} from 'electron-redux/renderer';
import {preload} from 'electron-redux/preload';

import reducer from './reducers/reducers';

preload();

const store = createStore(
    reducer,
    stateSyncEnhancer()
);

export default store;