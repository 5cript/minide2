const initialState = 
{
    bindings: {
        save: {key: 's', ctrl: true, shift: false, alt: false},
        saveAll: {key: 's', ctrl: true, shift: true, alt: false}
    }
};

module.exports = function reducer(state = initialState, action) 
{
    switch (action.type) 
    {
        case('SET_ALL_KEY_BINDINGS'): 
        {
            return {
                bindings: action.payload.bindings
            }
        }
        default:
            return state;
    }
}