class Router
{
    constructor(store)
    {
        this.store = store;
    }

    url(url)
    {
        return this.getHost() + url;
    }
    getHost()
    {
        let state = this.store.getState();
        return "http://" + state.backend.ip + ":" + state.backend.port;
    }
    setControlId(controlId)
    {
        this.controlId = controlId;
    }
    setDataId(dataId)
    {
        this.dataId = dataId;
    }
}

export default Router;