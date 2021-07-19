import {createStore} from 'redux';
import {stateSyncEnhancer, composeWithStateSync} from 'electron-redux/renderer';

import reducer from './reducers/reducers';

window.__ElectronReduxBridge = {
    stateSyncEnhancer: () => {},
    composeWithStateSync
}

const store = createStore(
    reducer,
    stateSyncEnhancer()
);

export default store;