export function initializeToolbars(objectFromServer) 
{
    let toolbars = {}
    let lookup = []
    let ot = objectFromServer.toolbars;
    for (const i in ot)
    {
        const toolbar = ot[i];
        lookup.push(toolbar.id);
        toolbars[toolbar.id] = {
            items: toolbar.items,
            name: toolbar.name,
            id: toolbar.id
        }
    }
    return {
        type: 'INITIALIZE_TOOLBARS',
        toolbars: toolbars,
        lookup: lookup
    }
}

export function setActiveToolbar(toolbarId)
{
    return {
        type: 'SET_ACTIVE_TOOLBAR',
        activeToolbar: toolbarId
    }
}

export function setItemsEnableStatus(toolbarId, itemIdArray, enabled)
{
    return {
        type: 'SET_TOOLBAR_ITEMS_ENABLED',
        toolbarId: toolbarId,
        itemIds: itemIdArray,
        enabled: enabled
    }
}

export function setItemRunning(toolbarId, itemId, running)
{
    return {
        type: 'SET_TOOLBAR_ITEM_RUNNING',
        toolbarId: toolbarId,
        itemId: itemId,
        running: running
    }
}