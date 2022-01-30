import ApiBase from './apibase'

class Environment extends ApiBase
{
    constructor({store, persistence, errorCallback, impl})
    {
        super(store, persistence, impl);
        this.errorCallback = errorCallback;
    }

    loadAll = async () =>
    {       
        return this.writeMessage("/api/settings/environment/load");
    }

    saveAll = async (environments) =>
    {
        return this.writeMessage("/api/settings/environment/save", {
            'environments': environments
        });
    }
}

export default Environment;