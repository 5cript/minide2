import {combineReducers} from 'redux';

import openFiles from './open_files';
import workspace from './workspace';

export default combineReducers({
    openFiles,
    workspace
})