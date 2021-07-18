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

export function setDebugInitialLoadDone()
{
    return {
        type: 'SET_DEBUG_LOAD_COMPLETE',
        payload: {}
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

export function addDebuggerInstance(instanceId)
{
    return {
        type: 'ADD_DEBUG_INSTANCE',
        payload: {
            instanceId: instanceId
        }
    }
}

export function removeDebuggerInstance(instanceId)
{
    return {
        type: 'REMOVE_DEBUG_INSTANCE',
        payload: {
            instanceId: instanceId
        }
    }
}

export function debuggerConsoleStream(instanceId, data)
{
    return {
        type: 'DEBUGGER_CONSOLE_STREAM',
        payload: {
            instanceId: instanceId,
            data: data
        }
    }
}

export function debuggerAddLibrary(instanceId, library)
{
    return {
        type: 'DEBUGGER_ADD_LIBRARY',
        payload: {
            instanceId: instanceId,
            library: library
        }
    }
}

export function debuggerThreadCreated(instanceId, thread)
{
    return {
        type: 'DEBUGGER_ADD_THREAD',
        payload: {instanceId, thread}
    }
}

export function debuggerThreadExit(instanceId, threadId)
{
    return {
        type: 'DEBUGGER_THREAD_EXIT',
        payload: {instanceId, threadId}
    }
}

export function debuggerSetProcessLife(instanceId, alive)
{
    return {
        type: 'DEBUGGER_SET_PROCESS_LIFE',
        payload: {instanceId, alive}
    }
}

export function debuggerSetAlive(instanceId, alive)
{
    return {
        type: 'DEBUGGER_SET_ALIVE',
        payload: {instanceId, alive}
    }
}

export function debuggerSetProcessExitCode(instanceId, code)
{
    return {
        type: 'DEBUGGER_SET_PROCESS_EXIT_CODE',
        payload: {instanceId, code}
    }
}