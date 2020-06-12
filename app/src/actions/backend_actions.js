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
        type: 'SET_CONNECTION_STATUS',
        ip: ip
    }
}

export function setBackendPort(port) 
{
    return {
        type: 'SET_CONNECTION_STATUS',
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