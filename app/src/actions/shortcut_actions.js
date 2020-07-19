export function setAllKeybinds(bindings) 
{
    return {
        type: 'SET_ALL_KEY_BINDINGS',
        bindings: bindings
    }
}