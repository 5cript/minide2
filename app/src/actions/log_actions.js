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

export function setLogType(name, type) 
{
    return {
        type: 'SET_LOG_TYPE',
        logName: name,
        logType: type
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

export function moveLogs(from, to)
{
    return {
        type: 'SWAP_LOGS',
        from: from,
        to: to
    }
}