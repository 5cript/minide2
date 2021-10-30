class ApiBase
{
    constructor(store, writeMessage)
    {
        this.store = store;
        this.writeMessage = writeMessage;
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