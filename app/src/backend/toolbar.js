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
            this.store.dispatch(toolbarInitialized(
                payload
            ))
        });
    }

    loadAll = async () =>
    {
        // FIXME: not needed anymore i think
        return this.writeMessage(`/api/toolbar/loadAll`);
    }

    callAction = async (toolbarId, itemId) =>
    {
        return this.writeMessage(`/api/toolbar/${toolbarId}/callAction`, {itemId});
    }

    cancelAction = async (toolbarId, itemId, force) =>
    {
        return this.writeMessage(`/api/toolbar/${toolbarId}/cancelAction`, {
            itemId,
            force
        });
    }

    menuAction = async (toolbarId, itemId, label) =>
    {
        return this.writeMessage(`/api/toolbar/${toolbarId}/menuAction`, {
            itemId,
            menuEntryLabel: label
        });
    }

    loadCombobox = async (toolbarId, itemId) =>
    {
        return this.writeMessage(`/api/toolbar/${toolbarId}/loadCombobox`, {
            itemId
        });
    }

    comboxSelect = async (toolbarId, itemId, selected) =>
    {
        return this.writeMessage(`/api/toolbar/${toolbarId}/comboboxSelect`, {
            itemId,
            selected
        })
    }

    logDoubleClick = async (toolbarId, logName, lineNumber, lineString) =>
    {
        // FIXME: Whats this?
        return this.writeMessage(`/api/toolbar/${toolbarId}/logDoubleClick`, {
            logName,
            lineNumber,
            lineString
        });
    }
}

export default ToolbarApi;