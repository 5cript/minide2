const initialState = 
{
    language: 'de_DE'
};

export default function reducer(state = initialState, action) 
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