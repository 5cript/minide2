import _ from 'lodash';

export default function reducer(state={
    openFiles: [
        /*
        {
            path:
            content:
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
        default:
            return state;
    }
}