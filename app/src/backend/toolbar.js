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

    callAction(toolbarId, itemId, onSuccess)
    {
        this.postJson(this.url("/api/toolbar/callAction"), {
            toolbarId: toolbarId,
            itemId: itemId
        }, (result) => {
            result.json().then(json => {
                if (onSuccess)
                    onSuccess(json);
            }).catch(err => {
                console.error('action result should have been json', err);
            })
        }, (err) => {
            console.error(err);
        })
    }

    cancelAction(toolbarId, itemId, force, onSuccess)
    {
        this.postJson(this.url("/api/toolbar/cancelAction"), {
            toolbarId: toolbarId,
            itemId: itemId,
            force: force
        }, (result) => {
            result.json().then(json => {
                if (onSuccess)
                    onSuccess(json);
            }).catch(err => {
                console.error('cancel result should have been json', err);
            })
        }, (err) => {
            console.error(err);
        })
    }

    menuAction(toolbarId, itemId, label)
    {
        this.postJson(this.url("/api/toolbar/menuAction"), {
            toolbarId: toolbarId,
            itemId:  itemId,
            menuEntryLabel: label
        })
    }

    loadCombobox(toolbarId, itemId)
    {
        this.postJson(this.url("/api/toolbar/loadCombobox"), {
            toolbarId: toolbarId,
            itemId: itemId
        })
    }

    comboxSelect(toolbarId, itemId, selected)
    {
        this.postJson(this.url("/api/toolbar/comboboxSelect"), {
            toolbarId: toolbarId,
            itemId: itemId,
            selected: selected
        })
    }

    logDoubleClick(toolbarId, logName, lineNumber, lineString)
    {
        this.postJson(this.url("/api/toolbar/logDoubleClick"), {
            toolbarId: toolbarId,
            logName: logName,
            lineNumber: lineNumber,
            lineString: lineString
        });
    }
}

export default ToolbarApi;