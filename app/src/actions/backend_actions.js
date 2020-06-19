export function setConnected(connected) 
{
    return {
        type: 'SET_CONNECTION_STATUS',
        connected: connected
    }
}

export function setBackendIp(ip) 
{
    return {
        type: 'SET_IP',
        ip: ip
    }
}

export function setBackendPort(port) 
{
    return {
        type: 'SET_PORT',
        port: port
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
        message: msg
    }
}

export function setTryingToConnect(connecting) 
{
    return {
        type: 'SET_TRY_TO_CONNECT',
        connecting: connecting
    }
}

export function setSessionId(id) 
{
    return {
        type: 'SET_SESSION_ID',
        sessionId: id
    }
}