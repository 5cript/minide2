const _ = require('lodash');

module.exports = function reducer(state={
    // contains all logs        
    logs: [{
        logName: 'Terminal',
        data: '',
        logType: '_terminal',
        closeable: false
    }],

    // what log is tab-selected?
    activeLog: 0,

    ordering: [0]
}, action) {
    switch (action.type) 
    {
        case 'ADD_TO_LOG': 
        {
            let logs = _.cloneDeep(state.logs);
            const logIndex = logs.findIndex(log => log.logName === action.payload.logName);
            let ordering = [...state.ordering];
            if (logIndex === -1)
            {
                logs.push({
                    data: action.payload.data,
                    logName: action.payload.logName
                })
                ordering.push(logs.length - 1);
            }
            else 
            {
                if (logs[logIndex].data === undefined)
                    logs[logIndex].data = action.payload.data;
                else
                    logs[logIndex].data += action.payload.data;
            }

            return {
                ...state,
                logs: logs,
                ordering: ordering
            }
        }
        case 'ADD_DEBUG_TERMINAL': 
        {
            let logs = _.cloneDeep(state.logs);
            let ordering = [...state.ordering];
            logs.push({
                logName: action.payload.name,
                data: action.payload.data ? action.payload.data : '',
                logType: '_debug_terminal',
                closeable: action.payload.closeable,
                props: action.payload.props,
                instanceId: action.payload.instanceId
            })
            ordering.push(logs.length - 1);
            return {
                ...state,
                logs: logs,
                ordering: ordering,
                activeLog: logs.length - 1
            }
        }
        case 'SET_ACTIVE_LOG': 
        {
            return {
                ...state,
                activeLog: state.ordering.findIndex(elem => elem === action.payload.index)
            }
        }
        case 'SET_LOG_TYPE':
        {
            let logs = _.cloneDeep(state.logs);
            const logIndex = logs.findIndex(log => log.logName === action.payload.logName);
            let ordering = [...state.ordering];
            if (logIndex === -1)
            {
                logs.push({
                    data: '',
                    logName: action.payload.logName,
                    logType: action.payload.logType
                })
                ordering.push(logs.length - 1);
            }
            else
            {
                logs[logIndex].logType = action.payload.logType
            }

            return {
                ...state,
                logs: logs,
                ordering: ordering
            }
        }
        case 'CLEAR_LOG': 
        {
            let logs = _.cloneDeep(state.logs);
            const logIndex = logs.findIndex(log => log.logName === action.payload.logName);
            let ordering = [...state.ordering];
            if (logIndex === -1)
            {
                logs.push({
                    data: '',
                    logName: action.payload.logName
                })
                ordering.push(logs.length - 1);
            }
            else
            {
                logs[logIndex].data = ''
            }

            return {
                ...state,
                logs: logs,
                ordering: ordering
            }
        }
        case 'SET_ACTIVE_LOG_BY_NAME': 
        {
            const logIndex = state.logs.findIndex(log => log.logName === action.payload.logName);

            if (logIndex === -1)
                return state;

            return {
                ...state,
                activeLog: logIndex
            }
        }
        case 'SWAP_LOGS':
        {
            if (!(action.payload.from >= 0 && action.payload.from < state.logs.length && action.payload.to >= 0 && action.payload.to < state.logs.length))
                return state;

            let ordering = [...state.ordering];            
            ordering.splice(action.payload.to, 0, ordering.splice(action.payload.from, 1)[0]);

            return {
                ...state,
                ordering: ordering
            }
        }
        default:
            return state;
    }
}