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
        default:
            return state;
    }
}