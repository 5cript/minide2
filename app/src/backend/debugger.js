import ApiBase from './apibase'

class DebuggerRouter extends ApiBase
{
    constructor({store, persistence, errorCallback, writeMessage})
    {
        super(store, persistence, writeMessage);
        this.errorCallback = errorCallback;
    }

    startDebugger = async (runProfile) =>
    {
        return this.writeMessage("/api/debugger/createInstance", {
            runProfile: runProfile
        });
    }

    sendCommand = async ({instanceId, command, params, token, options}) =>
    {
        return this.writeMessage("/api/debugger/command", {
            instanceId, command: {
                operation: command,
                token: token,
                params: params,
                options: options
            }
        });
    }

    sendRawCommand = async ({instanceId, command}) =>
    {
        return this.writeMessage("/api/debugger/rawCommand", {instanceId, command});
    }
}

export default DebuggerRouter;