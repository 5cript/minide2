export default function reducer(state={
    toolbars: {
        toolbars: {},
        lookup: []
    },
    activeToolbar: ''
}, action) 
{
    switch (action.type) 
    {
        case 'INITIALIZE_TOOLBARS': 
        {
            return {...state, toolbars: action.toolbars, lookup: action.lookup}
        }
        case 'SET_ACTIVE_TOOLBAR':
        {
            return {...state, activeToolbar: action.activeToolbar}
        }
        default:
            return state;
    }
}