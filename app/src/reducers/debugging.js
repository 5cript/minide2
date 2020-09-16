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
    initialLoadDone: false
}, action) 
{ 
    switch (action.type) 
    {
        case('SET_RUN_CONFIG'):
        {
            return {...state, selectedConfig: action.payload.runConfigName}
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

            let profiles = _.clone(state.profiles);
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
        default:
            return state;
    }
}