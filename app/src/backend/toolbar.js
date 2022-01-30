import Router from './apibase';
import {toolbarInitialized} from '../actions/toolbar_actions';

class ToolbarApi extends Router
{
    constructor({store, persistence, errorCallback, impl})
    {
        super(store, persistence, impl);
        this.errorCallback = errorCallback;

        this.registerEventListeners();
    }

    registerEventListeners = () => {
        this.impl.registerEventListener('toolbarInitialized', (payload) => {
            console.log(payload);
            this.store.dispatch(toolbarInitialized(
                payload
            ))
        });
    }

    loadAll = async () =>
    {
        return this.writeMessage("/api/toolbar/loadAll");
    }

    callAction = async (toolbarId, itemId) =>
    {
        return this.writeMessage("/api/toolbar/callAction", {
            toolbarId: toolbarId,
            itemId: itemId
        });
    }

    cancelAction = async (toolbarId, itemId, force) =>
    {
        return this.writeMessage("/api/toolbar/cancelAction", {
            toolbarId: toolbarId,
            itemId: itemId,
            force: force
        });
    }

    menuAction = async (toolbarId, itemId, label) =>
    {
        return this.writeMessage("/api/toolbar/menuAction", {
            toolbarId: toolbarId,
            itemId:  itemId,
            menuEntryLabel: label
        });
    }

    loadCombobox = async (toolbarId, itemId) =>
    {
        return this.writeMessage("/api/toolbar/loadCombobox", {
            toolbarId: toolbarId,
            itemId: itemId
        });
    }

    comboxSelect = async (toolbarId, itemId, selected) =>
    {
        return this.writeMessage("/api/toolbar/comboboxSelect", {
            toolbarId: toolbarId,
            itemId: itemId,
            selected: selected
        })
    }

    logDoubleClick = async (toolbarId, logName, lineNumber, lineString) =>
    {
        return this.writeMessage("/api/toolbar/logDoubleClick", {
            toolbarId: toolbarId,
            logName: logName,
            lineNumber: lineNumber,
            lineString: lineString
        });
    }
}

export default ToolbarApi;