export default function reducer(state={
    root: '',
    fileTree: {
        name: '',
        toogled: true,
        children: []
    }
}, action) {
    switch (action.type) {
        case 'FETCH_ROOT': {
            return {...state}
        }
        case 'OPEN_WORKSPACE': {
            return {...state, root: action.payload, fileTree: action.tree}
        }
        default:
            return state;
    }
}