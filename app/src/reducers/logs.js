import _ from 'lodash'

export default function reducer(state={
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
            const logIndex = logs.findIndex(log => log.logName === action.logName);
            let ordering = [...state.ordering];
            if (logIndex === -1)
            {
                logs.push({
                    data: action.data,
                    logName: action.logName
                })
                ordering.push(logs.length - 1);
            }
            else 
            {
                if (logs[logIndex].data === undefined)
                    logs[logIndex].data = action.data;
                else
                    logs[logIndex].data += action.data;
            }

            return {
                ...state,
                logs: logs,
                ordering: ordering
            }
        }
        case 'SET_ACTIVE_LOG': 
        {
            return {
                ...state,
                activeLog: state.ordering.findIndex(elem => elem === action.index)
            }
        }
        case 'SET_LOG_TYPE':
        {
            let logs = _.cloneDeep(state.logs);
            const logIndex = logs.findIndex(log => log.logName === action.logName);
            let ordering = [...state.ordering];
            if (logIndex === -1)
            {
                logs.push({
                    data: '',
                    logName: action.logName,
                    logType: action.logType
                })
                ordering.push(logs.length - 1);
            }
            else
            {
                logs[logIndex].logType = action.logType
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
            const logIndex = logs.findIndex(log => log.logName === action.logName);
            let ordering = [...state.ordering];
            if (logIndex === -1)
            {
                logs.push({
                    data: '',
                    logName: action.logName
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
            const logIndex = state.logs.findIndex(log => log.logName === action.logName);

            if (logIndex === -1)
                return state;

            return {
                ...state,
                activeLog: logIndex
            }
        }
        case 'SWAP_LOGS':
        {
            if (!(action.from >= 0 && action.from < state.logs.length && action.to >= 0 && action.to < state.logs.length))
                return state;

            let ordering = [...state.ordering];            
            ordering.splice(action.to, 0, ordering.splice(action.from, 1)[0]);

            return {
                ...state,
                ordering: ordering
            }
        }
        default:
            return state;
    }
}