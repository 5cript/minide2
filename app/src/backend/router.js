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
    postJson(url, id, data, onSuccess, onFailure)
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
        ).then(res => 
        {
            if (res.status >= 400) 
            {
                if (onFailure !== undefined) {
                    res.text().then(body => {
                        onFailure(body);   
                    }).catch(r => {
                        onFailure(res);
                    })
                }
                else
                    res.text().then((value) => {
                        this.errorCallback(value);
                    });
                return;
            }
            if (res.status >= 200 && res.status <= 299) 
            {
                if (onSuccess !== undefined)
                    onSuccess(res);
                return;
            }
        }).catch(e => 
        {
            if (onFailure !== undefined)
                onFailure(e.message);
            else
                console.error(e);
        });
    }
    getJsonWithCallbacks(urlPart, onSuccess, onError)
    {        
        let url = this.url(urlPart);
        fetch(
            url,
            {
                method: 'GET'
            }
        ).then(res => {
            if (res.status >= 300) {
                res.text().then((value) => {
                    onError(value);
                });
                return;
            }
            res.json().then(json => {
                onSuccess(json);
            })
        }).catch(e => {
            if (onError !== undefined)
                onError(e.message);
            else
                console.error(e);
        });
    }
}

export default Router;