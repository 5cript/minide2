export function addToLog(name, content) 
{
    return {
        type: 'ADD_TO_LOG',
        logName: name,
        data: content
    }
}

export function setActiveLog(index) 
{
    return {
        type: 'SET_ACTIVE_LOG',
        index: index
    }
}

export function clearLog(name) 
{
    return {
        type: 'CLEAR_LOG',
        logName: name
    }
}

export function focusLogByName(name)
{
    return {
        type: 'SET_ACTIVE_LOG_BY_NAME',
        logName: name
    }
}