module.exports = function reducer(state={
    selectedConfig: ''
}, action) 
{ 
    switch (action.type) 
    {
        case('SET_RUN_CONFIG'):
        {
            return {...state, selectedConfig: action.payload.runConfigName}
        }
        default:
            return state;
    }
}