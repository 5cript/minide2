export function setRunConfig(name)
{
    return {
        type: 'SET_RUN_CONFIG',
        payload: {
            runConfigName: name
        }
    };
}

export function setDebuggingProfiles(profiles)
{
    return {
        type: 'SET_DEBUG_PROFILES',
        payload: {
            profiles: profiles
        }
    }
}

export function setDebuggingProfile(name, profile, merge)
{
    return {
        type: 'SET_DEBUG_PROFILE',
        payload: {
            name: name,
            profile: profile,
            merge: merge
        }
    }
}

export function setGlobalDebuggerSettigns(settings)
{
    return {
        type: 'SET_GLOBAL_DEBUG_SETTINGS',
        payload: {
            settings: settings
        }
    }
}