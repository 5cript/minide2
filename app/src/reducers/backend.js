export default function reducer(state={
    controlId: -1,
    dataId: -1,
    ip: "[::1]",
    port: 43255
}, action) {
    switch (action.type) {
        case 'SET_CONTROL_ID': {
            return {...state, controlId: action.payload}
        }
        case 'SET_DATA_ID': {
            return {...state, dataId: action.payload}
        }
        case 'GET_CONTROL_ID': {
            return {controlId: state.controlId}
        }
        case 'GET_DATA_ID': {
            return {dataId: state.dataId}
        }
        case 'GET_BACKEND_IP': {
            return {ip: state.backendIp}
        }
        case 'GET_BACKEND_PORT': {
            return {port: state.backendPort}
        }
        default:
            return state;
    }
}