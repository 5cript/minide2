const initialState = 
{
    bindings: {
        save: {key: 's', ctrl: true, shift: false, alt: false},
        saveAll: {key: 's', ctrl: true, shift: true, alt: false}
    }
};

export default function reducer(state = initialState, action) 
{
    switch (action.type) 
    {
        case('SET_ALL_KEY_BINDINGS'): 
        {
            return {
                bindings: action.bindings
            }
        }
        default:
            return state;
    }
}