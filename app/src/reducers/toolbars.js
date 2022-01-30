const _ = require('lodash');

module.exports = function reducer(state={
    toolbars: {/*
        name: '';
        items: [{
            id: 'uniqueName',
            type // what is this element?
            [IconButton] pngbase64 // an image
            [IconButton] action // an action on the server when the button is pressed (if its a button)
            [IconButton] disables // an array of other ids of this toolbar to disable
            [IconButton] cancelable // is this cancelable by another click?
            [IconButton] running // is this running?
            [ComboBox] load // called when items are to be loaded to the client.
        }]
    */
    },
    activeToolbar: ''
}, {type, payload}) 
{
    switch (type) 
    {
        case 'TOOLBAR_INITIALIZED':
        {
            let toolbars = _.cloneDeep(state.toolbars);
            toolbars[payload.id] = {
                name: payload.name,
                items: payload.items
            }
            return {
                ...state, toolbars, activeToolbar: payload.id
            };
        }
        case 'SET_ACTIVE_TOOLBAR':
        {
            const active = payload.activeToolbar ? payload.activeToolbar : '';
            return {...state, activeToolbar: active}
        }
        case 'SET_TOOLBAR_ITEMS_ENABLED':
        {
            let toolbars = _.cloneDeep(state.toolbars);
            let toolbar = toolbars[payload.toolbarId];
            if (toolbar === undefined)
            {
                console.error('state update with invalid toolbar id, SET_TOOLBAR_ITEMS_ENABLED');
                return state;
            }

            toolbar.items = toolbar.items.map(item => 
            {
                const disabled = payload.itemIds.findIndex(it => it === item.id) !== -1 ? !payload.enabled : undefined;
                return{
                    ...item,
                    disabled: disabled
                }
            })

            return {
                ...state,
                toolbars: toolbars
            };
        }
        case 'SET_TOOLBAR_ITEM_RUNNING':
        {
            let toolbars = _.cloneDeep(state.toolbars);
            let toolbar = toolbars[payload.toolbarId];
            if (toolbar === undefined)
            {
                console.error('state update with invalid toolbar id, SET_TOOLBAR_ITEM_RUNNING');
                return state;
            }

            let item = toolbar.items.find(item => item.id === payload.itemId);
            if (item === undefined)
            {
                console.error('state update with invalid toolbar-item id, SET_TOOLBAR_ITEM_RUNNING');
                return state;
            }

            item.running = payload.running;

            return {
                ...state,
                toolbars: toolbars
            };
        }
        default:
            return state;
    }
}