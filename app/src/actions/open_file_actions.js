export function addOpenFile(file) 
{
    return {
        type: 'ADD_OPEN_FILE',
        payload: {
            file: {
                path: file,
                content: '',
                synchronized: false
            }
        }
    }
}

export function addOpenFileWithContent(file, isAbsolutePath, content) 
{
    return {
        type: 'ADD_OPEN_FILE',
        payload: {
            file: {
                path: file,
                content: content,
                isAbsolutePath: isAbsolutePath,
                synchronized: true
            },
            focus: true
        }
    }
}

export function removeOpenFile(file) 
{
    return {
        type: 'REMOVE_OPEN_FILE',
        payload: {
            path: file
        }
    }
}

export function setActiveFile(index) 
{
    return {
        type: 'SET_ACTIVE_FILE',
        payload: index
    }
}

export function setActiveFileContent(data) 
{
    return {
        type: 'SET_ACTIVE_FILE_CONTENT',
        payload: {
            content: data
        }
    }
}

export function activeFileWasSynchronized()
{
    return {
        type: 'ACTIVE_FILE_WAS_SYNCHRONIZED',
        payload: {} // empty payload for electron-redux
    };
}

export function fileWasSynchronized(path)
{
    return {
        type: 'FILE_WAS_SYNCHRONIZED',
        payload: {
            path: path
        }
    }
}

export function moveOpenFile(from, to)
{
    return {
        type: 'MOVE_OPEN_FILE',
        payload: {
            from: from,
            to: to
        }
    }
}