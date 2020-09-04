const initialState = 
{
    language: 'de_DE'
};

module.exports = function reducer(state = initialState, action) 
{
    switch (action.type) 
    {
        case('CHANGE_LANGUAGE'):
        {
            return {...state, language: action.language}
        }
        default:
            return state;
    }
}