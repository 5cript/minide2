import {combineReducers} from 'redux';

import openFiles from './open_files';
import workspace from './workspace';
import backend from './backend';

export default combineReducers({
    openFiles,
    workspace,
    backend
})