export default function reducer(state={
    toolbars: {
        toolbars: {},
        lookup: []
    }
}, action) {
    switch (action.type) {
        case 'INITIALIZE_TOOLBARS': 
        {
            return {...state, toolbars: action.toolbars, lookup: action.lookup}
        }
        default:
            return state;
    }
}