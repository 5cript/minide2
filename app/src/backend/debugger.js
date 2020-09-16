import Router from './router'

class DebuggerRouter extends Router
{
    constructor(state, errorCallback)
    {
        super(state);
        this.errorCallback = errorCallback;
    }

    startDebugger(runProfile, onSuccess, onError)
    {
        this.postJson(
            this.url("/api/debugger/createInstance"), 
            {
                runProfile: runProfile
            },
            response => {
                response.json().then(json => {
                    onSuccess(json);
                }).catch(err => {
                    console.error('createInstance result should have been json', err);
                })
            },
            err => {
                onError(err);
            }
        );
    }
}

export default DebuggerRouter;