import Router from './router'

class DebuggerRouter extends Router
{
    constructor(state, errorCallback)
    {
        super(state);
        this.errorCallback = errorCallback;
    }

    startDebugger(runProfile)
    {
        return new Promise((resolve, reject) => {
            this.postJson(
                this.url("/api/debugger/createInstance"), 
                {
                    runProfile: runProfile
                },
                response => {
                    response.json().then(json => {
                        resolve(json);
                    }).catch(err => {
                        reject({
                            message: 'createInstance result should have been json', 
                            error: err
                        });
                    })
                },
                err => {
                    reject(err);
                }
            );
        });
    }

    sendCommand({instanceId, command, params, token, options}) 
    {
        let commandObj = {instanceId, command: {operation: command}};
        if (token)
            commandObj.command.token = token;
        if (params)
            commandObj.command.params = params;
        if (options)
            commandObj.command.options = options;

        console.log(commandObj)

        return new Promise((resolve, reject) => {
            this.postJson(
                this.url("/api/debugger/command"),
                commandObj,
                response => {
                    resolve();
                },
                err => {
                    reject(err);
                }
            );
        })
    }

    sendRawCommand({instanceId, command}) 
    {
        let commandObj = {instanceId, command};

        return new Promise((resolve, reject) => {
            this.postJson(
                this.url("/api/debugger/rawCommand"),
                commandObj,
                response => {
                    resolve();
                },
                err => {
                    reject(err);
                }
            );
        })
    }
}

export default DebuggerRouter;