import Router from './router'

class DebuggerRouter extends Router
{
    constructor(state, errorCallback)
    {
        super(state);
        this.errorCallback = errorCallback;
    }

    startDebugger(runConfigName)
    {
        this.postJson(this.url("/api/debugger/createInstance"), {
            runConfigName: runConfigName
        });
    }
}

export default DebuggerRouter;