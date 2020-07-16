import _ from 'lodash'

export default function reducer(state={
    // will be populated like so:
    /*
        cmake: {
            data: '',
        }
    */
    logs: [{
        logName: 'Terminal',
        data: '',
        logType: '_terminal'
    }],
    otherLogState: 
    {
        // 0 is the terminal
        activeLog: 0
    }
}, action) {
    switch (action.type) 
    {
        case 'ADD_TO_LOG': 
        {
            let logs = _.cloneDeep(state.logs);
            const logIndex = logs.findIndex(log => log.logName === action.logName);
            if (logIndex === -1)
            {
                logs.push({
                    data: action.data,
                    logName: action.logName
                })
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
                logs: logs
            }
        }
        case 'SET_ACTIVE_LOG': 
        {
            let otherLogState = _.cloneDeep(state.otherLogState);
            otherLogState.activeLog = action.index;

            return {
                ...state,
                otherLogState
            }
        }
        case 'SET_LOG_TYPE':
        {
            let logs = _.cloneDeep(state.logs);
            const logIndex = logs.findIndex(log => log.logName === action.logName);
            if (logIndex === -1)
            {
                logs.push({
                    data: '',
                    logName: action.logName,
                    logType: action.logType
                })
            }
            else
            {
                logs[logIndex].logType = action.logType
            }

            return {
                ...state,
                logs: logs
            }
        }
        case 'CLEAR_LOG': 
        {
            let logs = _.cloneDeep(state.logs);
            const logIndex = logs.findIndex(log => log.logName === action.logName);
            if (logIndex === -1)
            {
                logs.push({
                    data: '',
                    logName: action.logName
                })
            }
            else
            {
                logs[logIndex].data = ''
            }

            return {
                ...state,
                logs: logs
            }
        }
        case 'SET_ACTIVE_LOG_BY_NAME': 
        {
            let otherLogState = _.cloneDeep(state.otherLogState);
            //otherLogState.activeLog = action.index;

            const logIndex = state.logs.findIndex(log => log.logName === action.logName);

            if (logIndex === -1)
                return state;

            otherLogState.activeLog = logIndex;

            return {
                ...state,
                otherLogState
            }
        }
        case 'SWAP_LOGS':
        {
            if (!(action.first >= 0 && action.first < state.logs.length && action.second >= 0 && action.second < state.logs.length))
                return state;

            let logs = _.cloneDeep(state.logs);            
            const log = logs[action.first];
            logs[action.first] = logs[action.second];
            logs[action.second] = log;

            return {
                ...state,
                logs: logs
            }
        }
        default:
            return state;
    }
}