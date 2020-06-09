import {combineReducers} from 'redux';

import openFiles from './open_files';
import workspace from './workspace';
import backend from './backend';
import shortcuts from './shortcuts';
import locale from './locale';

export default combineReducers({
    openFiles,
    workspace,
    backend,
    shortcuts,
    locale
})