class DebugController
{
    constructor(backend, store)
    {
        this.backend = backend;
        this.debugger = this.backend.debugger();
        this.store = store;
    }

    startDebugger = (profile) =>
    {
        this.debugger.startDebugger(profile)
    }
}

export default DebugController;