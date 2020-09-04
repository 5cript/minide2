module.exports = function reducer(state={
    ip: '',
    port: 0,
    connected: false,
    connectMessage: '',
    sessionId: '',
    tryingToConnect: false
}, action) {
    switch (action.type) {
        case 'SET_IP': {
            return {...state, ip: action.ip}
        }
        case 'SET_PORT': {
            return {...state, port: action.port}
        }
        case 'SET_CONNECTION_STATUS': {
            if (action.connected === true)
                return {...state, connected: action.connected}
            else
            return {...state, connected: action.connected, sessionId: ''}
        }
        case 'SET_TRY_TO_CONNECT': {
            return {...state, tryingToConnect: action.connecting}
        }
        case 'SET_CONNECT_MESSAGE': {
            return {...state, connectMessage: action.message}
        }
        case 'SET_SESSION_ID': {
            return {...state, sessionId: action.sessionId}
        }
        default:
            return state;
    }
}