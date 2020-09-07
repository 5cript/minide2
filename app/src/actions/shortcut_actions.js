export function setAllKeybinds(bindings) 
{
    return {
        type: 'SET_ALL_KEY_BINDINGS',
        payload: {
            bindings: bindings
        }
    }
}
export function setKeybind(name, binding) 
{
    return {
        type: 'SET_KEY_BIND',
        payload: {
            name: name,
            binding: binding
        }
    }
}