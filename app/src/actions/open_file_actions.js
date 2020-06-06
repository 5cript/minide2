export function addOpenFile(file) {
    return {
        type: 'ADD_OPEN_FILE',
        payload: file
    }
}

export function removeOpenFile(file) {
    return {
        type: 'REMOVE_OPEN_FILE',
        payload: file
    }
}

export function setActiveFile(index) {
    return {
        type: 'SET_ACTIVE_FILE',
        payload: index
    }
}