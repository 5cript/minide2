const initialState = 
{
    bindings: {
        save: {key: 's', ctrlKey: true, shiftKey: false, altKey: false},
        saveAll: {key: 's', ctrlKey: true, shiftKey: true, altKey: false}
    }
};

export default function reducer(state = initialState, action) 
{
    switch (action.type) 
    {
        default:
            return state;
    }
}