export function setPreferences(preferencesObject)
{
    return {
        type: 'SET_PREFERENCES',
        preferences: preferencesObject
    };
}