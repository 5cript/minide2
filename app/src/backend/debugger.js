import Router from './router'

class DebuggerRouter extends Router
{
    constructor(state, errorCallback)
    {
        super(state);
        this.errorCallback = errorCallback;
    }

    startDebugger(runProfileName)
    {
        this.postJson(
            this.url("/api/debugger/createInstance"), 
            {
                runProfileName: runProfileName
            },
            response => {
                response.json().then(json => {
                    console.log(json);
                }).catch(err => {
                    console.error('createInstance result should have been json', err);
                })
            },
            err => {
                console.error(err);
            }
        );
    }
}

export default DebuggerRouter;