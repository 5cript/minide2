class ApiBase
{
    constructor(store, writeMessage)
    {
        this.store = store;
        this.writeMessage = writeMessage;
    }

    authInfo = (obj) =>
    {
        if (obj === undefined)
            obj = {}
        
        if (obj.headers === undefined)
            obj.headers = {}

        const backend = this.store.getState().backend;
        if (backend.sessionId.length > 0)
            obj.headers["cookie"] = "aSID=" + backend.sessionId;
        else
            obj.headers["Authorization"] = "Basic " + btoa("admin:dummy");
        obj.credentials = 'include';
        return obj
    }
    tryParseJson = (maybeJson) =>
    {
        try
        {
            return JSON.parse(maybeJson)
        }
        catch(err)
        {
            return maybeJson;
        }
    }
    getHost = (protocol) =>
    {
        let state = this.store.getState();
        return protocol + "://" + state.backend.ip + ":" + state.backend.port;
    }
    send = async (type, payload) =>
    {
        return this.writeMessage(
            type,
            payload
        );   
    }
}

export default ApiBase;