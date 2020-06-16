import Router from './router'

class Environment extends Router
{
    constructor(state, errorCallback)
    {
        super(state);
        this.errorCallback = errorCallback;
    }

    loadAll(onSuccess, onError)
    {        
        this.getJsonWithCallbacks("/api/settings/environment/load", onSuccess, onError);
    }

    saveAll(environments, onSuccess, onError)
    {
        this.postJson(this.url("/api/settings/environment/save"), -1, {'environments': environments}, onSuccess, onError);
    }
}

export default Environment;