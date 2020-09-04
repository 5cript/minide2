export function setConnected(connected) 
{
    return {
        type: 'SET_CONNECTION_STATUS',
        payload: {
            connected: connected
        }
    }
}

export function setBackendIp(ip) 
{
    return {
        type: 'SET_IP',
        payload: {
            ip: ip
        }
    }
}

export function setBackendPort(port) 
{
    return {
        type: 'SET_PORT',
        payload: {
            port: port
        }
    }
}

/**
 * Used in blocker bar
 * @param msg message in blocker bar.
 */
export function setConnectMessage(msg) 
{
    return {
        type: 'SET_CONNECT_MESSAGE',
        payload: {
            message: msg
        }
    }
}

export function setTryingToConnect(connecting) 
{
    return {
        type: 'SET_TRY_TO_CONNECT',
        payload: {
            connecting: connecting
        }
    }
}

export function setSessionId(id) 
{
    return {
        type: 'SET_SESSION_ID',
        payload: {
            sessionId: id
        }
    }
}