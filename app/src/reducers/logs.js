import _ from 'lodash'

export default function reducer(state={
    // will be populated like so:
    /*
        cmake: {
            data: ''
        }
    */

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
            let update = {}
            if (state[action.logName] !== undefined)
                update[action.logName] = _.cloneDeep(state[action.logName]);
            else
                update[action.logName] = {data: ''}
            if (update[action.logName].data === undefined)
                update[action.logName].data = action.data;
            else
                update[action.logName].data += action.data;

            return {
                ...state,
                ...update
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
            let update = {}
            if (state[action.logName] !== undefined)
                update[action.logName] = _.cloneDeep(state[action.logName]);
            else
                update[action.logName] = {data: ''}

            update[action.logName].type = action.logType;

            return {
                ...state,
                ...update
            }
        }
        case 'CLEAR_LOG': 
        {
            let update = {}
            if (state[action.logName] !== undefined)
            {
                update[action.logName] = _.cloneDeep(state[action.logName]);
                update[action.logName].data = '';
            }
            else
                update[action.logName] = {data: ''}

            return {
                ...state,
                ...update
            }
        }
        case 'SET_ACTIVE_LOG_BY_NAME': 
        {
            let otherLogState = _.cloneDeep(state.otherLogState);
            //otherLogState.activeLog = action.index;

            let c = 0;
            let found = false;
            for (const [key, ] of Object.entries(state)) 
            {
                if (key === "otherLogState")
                    continue;
                if (key === action.logName)
                {
                    found = true;
                    break;
                }
                ++c;
            }

            if (!found)
                return state;

            otherLogState.activeLog = c + 1;

            return {
                ...state,
                otherLogState
            }
        }
        default:
            return state;
    }
}