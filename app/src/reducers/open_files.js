import _ from 'lodash';

export default function reducer(state={
    openFiles: [
        /*
        {
            path:
            content:
            isAbsolutePath: 
            synchronized:
        }
        */
    ],
    activeFile: -1
}, action) {
    switch (action.type) {
        case 'ADD_OPEN_FILE': {
            let activeFile = state.activeFile === -1 ? 0 : state.activeFile;

            let openFiles = _.clone(state.openFiles);
            let fileIndex = openFiles.findIndex(file => file.path === action.file.path);
            if (fileIndex === -1)
            {
                if (action.focus)
                    activeFile = state.openFiles.length;
                openFiles.push(action.file);
            }
            else
            {
                if (action.focus)
                    activeFile = fileIndex;
                openFiles[fileIndex].content = _.clone(action.file.content);
                openFiles[fileIndex].synchronized = _.clone(action.file.synchronized);
                openFiles[fileIndex].isAbsolutePath = _.clone(action.file.isAbsolutePath);
            }
            return {
                ...state, 
                openFiles: openFiles, 
                activeFile: activeFile
            }
        }
        case 'SET_ACTIVE_FILE': {
            return {...state, activeFile: action.payload}
        }
        case 'ACTIVE_FILE_WAS_SYNCHRONIZED': {
            if (state.activeFile === -1)
                return state;

            let openFiles = _.clone(state.openFiles);
            openFiles[state.activeFile].synchronized = true;

            return {
                ...state,
                openFiles: openFiles
            }
        }
        case 'FILE_WAS_SYNCHRONIZED': {
            let openFiles = _.clone(state.openFiles);
            let fileIndex = openFiles.findIndex(file => file.path === action.path);
            if (fileIndex === -1)
                console.log('tried to synchronize unopened file');
            else
                openFiles[fileIndex].synchronized = true;

            return {
                ...state,
                openFiles: openFiles
            }
        }
        case 'REMOVE_OPEN_FILE': {
            return {
                ...state, 
                openFiles: state.openFiles.filter(f => {
                    return f.path !== action.path
                }), 
                activeFile: (() => {
                    if (state.activeFile >= state.openFiles.length - 1)
                        return state.openFiles.length - 2;
                    if (state.activeFile === 0)
                        return 0;
                    if (state.openFiles.length === 1)
                        return -1;
                    return state.activeFile - 1;
                })()
            }
        }
        case 'SET_ACTIVE_FILE_CONTENT': {
            if (state.activeFile === -1)
                return state;
                
            let openFiles = _.clone(state.openFiles);
            openFiles[state.activeFile].content = action.content;
            openFiles[state.activeFile].synchronized = false;

            return {
                ...state,
                openFiles: openFiles
            }
        }
        case 'MOVE_OPEN_FILE': {
            if (!(action.from >= 0 && action.from < state.openFiles.length && action.to >= 0 && action.to < state.openFiles.length))
                return state;

            let lastActiveFilePath = '';
            if (state.activeFile !== -1)
                lastActiveFilePath = state.openFiles[state.activeFile].path;
            let openFiles = _.clone(state.openFiles);
            openFiles.splice(action.to, 0, openFiles.splice(action.from, 1)[0]);
            if (lastActiveFilePath !== '')
                return {
                    ...state,
                    openFiles: openFiles,
                    activeFile: openFiles.findIndex(file => file.path === lastActiveFilePath)
                };
            else
                return {
                    ...state,
                    openFiles: openFiles
                };
        }
        default:
            return state;
    }
}