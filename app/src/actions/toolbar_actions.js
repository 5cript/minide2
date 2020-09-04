export function initializeToolbars(toolbars, lookup) 
{
    return {
        type: 'INITIALIZE_TOOLBARS',
        payload: {
            toolbars: toolbars,
            lookup: lookup
        }
    }
}

export function setActiveToolbar(toolbarId)
{
    return {
        type: 'SET_ACTIVE_TOOLBAR',
        payload: {
            activeToolbar: toolbarId
        }
    }
}

export function setItemsEnableStatus(toolbarId, itemIdArray, enabled)
{
    return {
        type: 'SET_TOOLBAR_ITEMS_ENABLED',
        payload: {
            toolbarId: toolbarId,
            itemIds: itemIdArray,
            enabled: enabled
        }
    }
}

export function setItemRunning(toolbarId, itemId, running)
{
    return {
        type: 'SET_TOOLBAR_ITEM_RUNNING',
        payload: {
            toolbarId: toolbarId,
            itemId: itemId,
            running: running
        }
    }
}