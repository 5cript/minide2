export function addToLog(name, content) 
{
    return {
        type: 'ADD_TO_LOG',
        payload: {
            logName: name,
            data: content
        }
    }
}

export function setActiveLog(index) 
{
    return {
        type: 'SET_ACTIVE_LOG',
        payload: {
            index: index
        }
    }
}

export function setLogType(name, type) 
{
    return {
        type: 'SET_LOG_TYPE',
        payload: {
            logName: name,
            logType: type
        }
    }
}

export function clearLog(name) 
{
    return {
        type: 'CLEAR_LOG',
        payload: {
            logName: name
        }
    }
}

export function focusLogByName(name)
{
    return {
        type: 'SET_ACTIVE_LOG_BY_NAME',
        payload: {
            logName: name
        }
    }
}

export function moveLogs(from, to)
{
    return {
        type: 'SWAP_LOGS',
        payload: {
            from: from,
            to: to
        }
    }
}

export function addDebugTerminal(name, data, props, closeable, instanceId)
{
    return {
        type: 'ADD_DEBUG_TERMINAL',
        payload: {
            name: name,
            data: data,
            props: props,
            closeable: closeable,
            instanceId: instanceId
        }
    }
}

export function removeDebugTerminal(instanceId)
{
    return {
        type: 'REMOVE_DEBUG_TERMINAL',
        payload: {
            instanceId: instanceId
        }
    }
}