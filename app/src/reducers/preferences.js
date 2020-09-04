module.exports = function reducer(state={
    preferences: {}
}, action) 
{
    switch (action.type) 
    {
        case 'SET_PREFERENCES': 
        {
            return {
                ...state,
                preferences: action.payload.preferences
            }
        }
        default:
            return state;
    }
}