module.exports = function reducer(state={
    configHome: ''
}, action) 
{ 
    switch (action.type) 
    {
        case('SET_HOME'):
        {
            return {...state, configHome: action.payload.home}
        }
        default:
            return state;
    }
}