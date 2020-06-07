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
    postJson(url, id, data)
    {
        let body = {
            "id": id,
            ...data
        };
        fetch(
            url, 
            {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Accept': 'application/json'
                },
                body: JSON.stringify(body)
            }
        ).then(res => {
            if (res.status >= 300) {
                res.text().then((value) => {
                    this.errorCallback(value);
                });
                return;
            }
        }).catch(e => {
            console.error(e);
        });
    }
}

export default Router;