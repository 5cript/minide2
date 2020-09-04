module.exports = function reducer(state={
    ip: '',
    port: 0,
    connected: false,
    connectMessage: '',
    sessionId: '',
    tryingToConnect: false
}, action) 
{
    switch (action.type) 
    {
        case 'SET_IP': {
            return {...state, ip: action.payload.ip}
        }
        case 'SET_PORT': {
            return {...state, port: action.payload.port}
        }
        case 'SET_CONNECTION_STATUS': {
            if (action.payload.connected === true)
                return {...state, connected: action.payload.connected}
            else
            return {...state, connected: action.payload.connected, sessionId: ''}
        }
        case 'SET_TRY_TO_CONNECT': {
            return {...state, tryingToConnect: action.payload.connecting}
        }
        case 'SET_CONNECT_MESSAGE': {
            return {...state, connectMessage: action.payload.message}
        }
        case 'SET_SESSION_ID': {
            return {...state, sessionId: action.payload.sessionId}
        }
        default:
            return state;
    }
}