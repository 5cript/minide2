import {addDebugTerminal} from './actions/log_actions';

class DebugController
{
    constructor(backend, store)
    {
        this.backend = backend;
        this.debuggerBackend = this.backend.debugger();
        this.workspaceBackend = this.backend.workspace();
        this.store = store;
    }

    startDebugger = (debuggingState, onStart, onError) =>
    {
        this.workspaceBackend.loadRunConfig(
            (profiles) => {
                const profile = profiles.configurations.find(profile => profile.name === debuggingState.selectedConfig);

                if (profile.debugger === undefined)
                    return onError("$ProvideDebuggerProfile");

                const regexResult = ('' + profile.debugger).match(/\$\{((?:[^}])+)\}/);
                const capture = regexResult[1];

                if (capture === undefined)
                    return onError("$DebuggerProfileOfInvalidForm");

                const split = capture.split(':');

                if (split.length !== 2)
                    return onError("$DebuggerProfileOfInvalidForm");

                if (split[0] !== 'DebuggerProfile')
                    return onError("$DebuggerProfileOfInvalidForm");

                const debuggr = debuggingState.profiles.find(profile => profile.name === split[1]);
                
                if (debuggr === undefined)
                    return onError("$DebuggerProfileWithGivenNameNotFound");

                profile.debugger = debuggr;
                console.log(profile);

                this.debuggerBackend.startDebugger(profile, res => {
                    this.store.dispatch(addDebugTerminal("Debugger Terminal", "", {}, false, res.instanceId));
                }, err => {
                    onError(err);
                });
            },
            (err) => {
                onError(err);
            }
        )
    }

    onMessage = (message) =>
    {
        delete message.type;
        console.log(message);
    }
}

export default DebugController;