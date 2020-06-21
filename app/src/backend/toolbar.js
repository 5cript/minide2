import Router from './router';

class ToolbarApi extends Router
{
    constructor(state, errorCallback)
    {
        super(state);
        this.errorCallback = errorCallback;
    }

    loadAll(onSuccess)
    {
        this.postJson(this.url("/api/toolbar/loadAll"), {
        }, onSuccess);
    }

    callAction(toolbarId, itemId)
    {
        this.postJson(this.url("/api/toolbar/callAction"), {
            toolbarId: toolbarId,
            itemId: itemId
        })
    }
}

export default ToolbarApi;