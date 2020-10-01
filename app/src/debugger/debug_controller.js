// Actions
import {addDebugTerminal} from '../actions/log_actions';
import {addDebuggerInstance} from '../actions/debugging_actions';

// Other
import DebugInstance from './debug_instance';

class DebugController
{
    constructor(backend, store)
    {
        this.backend = backend;
        this.debuggerBackend = this.backend.debugger();
        this.workspaceBackend = this.backend.workspace();
        this.store = store;

        this.instances = {};
    }

    startDebugger = (debuggingState) =>
    {
        return new Promise((resolve, reject) => {
            this.workspaceBackend.loadRunConfig(
                (profiles) => {
                    const profile = profiles.configurations.find(profile => profile.name === debuggingState.selectedConfig);
    
                    if (profile.debugger === undefined)
                        return reject({
                            message: "$ProvideDebuggerProfile",
                            err: {}
                        });
    
                    const regexResult = ('' + profile.debugger).match(/\$\{((?:[^}])+)\}/);
                    const capture = regexResult[1];
    
                    if (capture === undefined)
                        return reject({
                            message: "$DebuggerProfileOfInvalidForm",
                            err: {}
                        });
    
                    const split = capture.split(':');
    
                    if (split.length !== 2)
                        return reject({
                            message: "$DebuggerProfileOfInvalidForm",
                            err: {}
                        });
    
                    if (split[0] !== 'DebuggerProfile')
                        return reject({
                            message: "$DebuggerProfileOfInvalidForm",
                            err: {}
                        });
    
                    const debuggr = debuggingState.profiles.find(profile => profile.name === split[1]);
                    
                    if (debuggr === undefined)
                        return reject({
                            message: "$DebuggerProfileWithGivenNameNotFound",
                            err: {}
                        });
    
                    profile.debugger = debuggr;
                    console.log(profile);
    
                    this.debuggerBackend.startDebugger(profile).then(res => {
                        this.store.dispatch(addDebuggerInstance(res.instanceId));
                        this.store.dispatch(addDebugTerminal("Debugger Terminal", "", {}, false, res.instanceId));
                        this.instances[res.instanceId] = new DebugInstance(this.backend, this.store, res.instanceId);
                        resolve(res);
                    }).catch(err => {
                        reject(err);
                    });
                },
                (err) => {
                    reject(err);
                }
            )
        });
    }

    sendCommand = (commandObject) =>
    {
        this.debuggerBackend.sendCommand(commandObject)
    }

    sendRawCommand = ({instanceId, command}) =>
    {
        this.debuggerBackend.sendRawCommand({instanceId, command});
    }

    onMessage = (message) =>
    {
        if (message.messageType !== 'console_stream' && message.messageType !== 'log_stream' && message.messageType !== 'notify_record')
            console.log(message);

        const instance = this.instances[message.instanceId];
        if (instance === undefined)
        {
            console.error('message received for missing instance');
            return;
        }

        switch(message.messageType)
        {
            case('console_stream'):
            {
                instance.onConsoleStream(message.data);
                break;
            }
            case('notify_record'):
            {
                instance.onNotifyRecord({
                    status: message.status,
                    type: message.type,
                    results: message.results
                });
                break;
            }
            case('log_stream'):
            {
                instance.onLogStream(message.data);
                break;
            }
            case('exec_record'): 
            {
                instance.onExecRecord({
                    status: message.status,
                    type: message.type,
                    results: message.results
                });
                break;
            }
            default:
                break;
        }
    }
}

export default DebugController;