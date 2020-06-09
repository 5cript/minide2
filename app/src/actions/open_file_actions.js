export function addOpenFile(file) 
{
    return {
        type: 'ADD_OPEN_FILE',
        file: {
            path: file,
            content: '',
            synchronized: false
        }
    }
}

export function addOpenFileWithContent(file, content) 
{
    return {
        type: 'ADD_OPEN_FILE',
        file: {
            path: file,
            content: content,
            synchronized: true
        },
        focus: true
    }
}

export function removeOpenFile(file) 
{
    return {
        type: 'REMOVE_OPEN_FILE',
        path: file
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
        content: data
    }
}

export function activeFileWasSynchronized()
{
    return {
        type: 'ACTIVE_FILE_WAS_SYNCHRONIZED'
    };
}