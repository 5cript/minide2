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

    menuAction(toolbarId, itemId, label)
    {
        this.postJson(this.url("/api/toolbar/menuAction"), {
            toolbarId: toolbarId,
            itemId,  itemId,
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