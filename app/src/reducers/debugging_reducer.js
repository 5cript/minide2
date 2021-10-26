const _ = require('lodash');

module.exports = function reducer(state={
    selectedConfig: '',
    globalSettings: {
        buildBeforeRun: false,
        runBinaryImmediately: true, // useful when there is a debugging console
        autoWatchLocals: false,
        autoWatchFunctionArguments: false
    },
    profiles: [/*{
        name: 'dummy',
        debugger: '${Profiles:blub}',
        path: '',
        fullyReadSymbols: true,
        neverReadSymbols: false,
        returnChildResult: true,
        initCommandFile: null, // can be null,
        commandFile: null, // can be null 
        ignoreGdbInit: true,
        ignoreAllGdbInit: true,
        additionCommandlineArguments: ''
    }*/],
    initialLoadDone: false,
    instances: {
        /*
        SOME_INSTANCE_ID: {
            instanceId: 'bla',
            consoleStream: 'bla\nbla',
            sessionData: 'default'
        }
         */
    },
    focussedInstance: null,
    sessionData: {
        'default': {
            breakpoints: [/*{
                file: '/test-project/main.cpp',
                line: 7
            }*/]
        }
    },
    activeSessionData: 'default'
}, action) 
{ 
    switch (action.type) 
    {
        case('SET_RUN_CONFIG'):
        {
            return {...state, selectedConfig: action.payload.runConfigName}
        }
        case('REMOVE_OPEN_FILE'):
        {
            let sessionData = _.cloneDeep(state.sessionData);
            for (const sessionId in sessionData)
            {
                sessionData[sessionId].breakpoints = sessionData[sessionId].breakpoints.filter(breakpoint =>
                    breakpoint.file !== action.payload.path
                );
            }
            return {...state, sessionData: sessionData};
        }
        case('SET_DEBUG_PROFILES'):
        {
            return {...state, profiles: action.payload.profiles}
        }
        case('SET_GLOBAL_DEBUG_SETTINGS'): 
        {
            return {
                ...state, 
                globalSettings: action.payload.settings,
            }
        }
        case('SET_DEBUG_LOAD_COMPLETE'):
        {
            return {
                ...state,
                initialLoadDone: true
            }
        }
        case('SET_DEBUG_PROFILE'):
        {
            if (action.payload.name === undefined)
                return state;

            let profiles = _.cloneDeep(state.profiles);
            let profileIndex = profiles.findIndex(profile => profile.name === action.payload.name);
            if (profileIndex === -1)
                return state;

            if (!action.payload.merge)
                profiles[profileIndex] = {
                    ...action.payload.profile,
                    name: action.payload.name
                };
            else
            {
                profiles[profileIndex] = {
                    ...profiles[profileIndex],
                    ...action.payload.profile,
                };

                for (let key in action.payload.profile)
                {
                    if (action.payload.profile[key] === null)
                        delete profiles[key];
                }
            }

            return {
                ...state,
                profiles: profiles
            }
        }
        case('ADD_DEBUG_INSTANCE'):
        {
            let instances = _.cloneDeep(state.instances);
            instances[action.payload.instanceId] = {
                instanceId: action.payload.instanceId,
                debuggerAlive: true,
                sessionData: 'initial',
            };
            return {
                ...state,
                instances: instances,
                focussedInstance: instances.length === 1 ? 0 : state.focussedInstance
            }
        }
        case('REMOVE_DEBUG_INSTANCE'):
        {
            let instances = _.cloneDeep(state.instances);
            delete instances[action.payload.instanceId];
            let focussed = instances.length === 0 ? -1 : state.focussedInstance;
            if (focussed >= instances.length)
                focussed = 0;
            return {
                ...state,
                instances: instances,
                focussedInstance: focussed
            }
        }
        case('DEBUGGER_CONSOLE_STREAM'):
        {
            const maybeInstance = state.instances[action.payload.instanceId];
            if (maybeInstance === undefined)
                return state;

            let instances = _.cloneDeep(state.instances);

            if (instances[action.payload.instanceId].consoleStream === undefined)
                instances[action.payload.instanceId].consoleStream = "";
            instances[action.payload.instanceId].consoleStream += action.payload.data;
            return {
                ...state,
                instances
            }
        }
        case('DEBUGGER_ADD_LIBRARY'):
        {
            let instances = _.cloneDeep(state.instances);
            if (instances[action.payload.instanceId] === undefined)
                return state;

            if (instances[action.payload.instanceId].libraries === undefined)
                instances[action.payload.instanceId].libraries = [];

            instances[action.payload.instanceId].libraries.push(action.payload.library)
            return {
                ...state,
                instances
            }
        }
        case('DEBUGGER_ADD_THREAD'):
        {
            let instances = _.cloneDeep(state.instances);
            if (instances[action.payload.instanceId] === undefined)
                return state;

            if (instances[action.payload.instanceId].threads === undefined)
                instances[action.payload.instanceId].threads = [];

            instances[action.payload.instanceId].threads.push(action.payload.thread)
            return {
                ...state,
                instances
            }
        }
        case('DEBUGGER_THREAD_EXIT'):
        {
            let instances = _.cloneDeep(state.instances);
            if (instances[action.payload.instanceId] === undefined)
                return state;

            if (instances[action.payload.instanceId].threads === undefined)
                return state;

            const threadIndex = instances[action.payload.instanceId].threads.findIndex(thread => thread.id === action.payload.threadId);
            if (threadIndex === -1)
                return state;

            instances[action.payload.instanceId].threads.slice(threadIndex, 1);

            return {
                ...state,
                instances
            }
        }
        case('DEBUGGER_SET_PROCESS_LIFE'):
        {
            let instances = _.cloneDeep(state.instances);
            if (instances[action.payload.instanceId] === undefined)
                return state;

            instances[action.payload.instanceId].alive = action.payload.alive;

            return {
                ...state,
                instances
            }
        }
        case('DEBUGGER_SET_ALIVE'):
        {
            let instances = _.cloneDeep(state.instances);
            if (instances[action.payload.instanceId] === undefined)
                return state;

            instances[action.payload.instanceId].debuggerAlive = action.payload.alive;

            return {
                ...state,
                instances
            }
        }
        case('DEBUGGER_SET_PROCESS_EXIT_CODE'):
        {
            let instances = _.cloneDeep(state.instances);
            if (instances[action.payload.instanceId] === undefined)
                return state;

            instances[action.payload.instanceId].exitCode = action.payload.code;

            return {
                ...state,
                instances
            }
        }
        case('DEBUGGER_TOGGLE_BREAKPOINT'):
        {
            let sessionData = _.cloneDeep(state.sessionData);
            let data = sessionData[action.payload.name]
            if (data === undefined)
                return state;

            if (data.breakpoints === undefined || data.breakpoints === null)
                data.breakpoints = []

            let breakpointIndex = data.breakpoints.findIndex(element => {
                return element.file === action.payload.file &&
                       element.line === action.payload.line;
            });

            if (breakpointIndex === -1)
                data.breakpoints.push({
                    file: action.payload.file,
                    line: action.payload.line
                });
            else
                data.breakpoints.splice(breakpointIndex, 1);

            sessionData[action.payload.name] = data;
            return {
                ...state,
                sessionData
            }
        }
        case('DEBUGGER_SET_FOCUSSED_INSTANCE'):
        {
            return {
                ...state,
                focussedInstance: state.instances.findIndex(instance => {
                    return instance.instanceId === action.payload.instanceId;
                })
            }
        }
        case('DEBUGGER_ADD_SESSION_DATA'):
        {
            // TODO
            return state;
        }
        case('DEBUGGER_REMOVE_SESSION_DATA'):
        {
            if (action.payload.name === state.defaultSessionDataInstance)
                return state;
            return state;
        }
        default:
            return state;
    }
}