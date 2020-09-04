export function setRunConfig(name)
{
    return {
        type: 'SET_RUN_CONFIG',
        payload: {
            runConfigName: name
        }
    };
}