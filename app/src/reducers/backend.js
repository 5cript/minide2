export default function reducer(state={
    ip: "[::1]",
    port: 43255,
    connected: false,
    connectMessage: '',
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
            return {...state, connected: action.connected}
        }
        case 'SET_TRY_TO_CONNECT': {
            return {...state, tryingToConnect: action.connecting}
        }
        case 'SET_CONNECT_MESSAGE': {
            return {...state, connectMessage: action.message}
        }
        default:
            return state;
    }
}