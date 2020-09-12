export function setConfigHome(home)
{
    return {
        type: 'SET_HOME',
        payload: {
            home: home
        }
    }
}