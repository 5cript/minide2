export function setPreferences(preferencesObject)
{
    return {
        type: 'SET_PREFERENCES',
        payload: {
            preferences: preferencesObject
        }
    };
}