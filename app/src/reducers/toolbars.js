const _ = require('lodash');

module.exports = function reducer(state={
    toolbars: {
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
        */},
        lookup: []
    },
    activeToolbar: ''
}, action) 
{
    switch (action.type) 
    {
        case 'INITIALIZE_TOOLBARS': 
        {
            console.log('initialize toolbars')
            return {...state, toolbars: action.toolbars, lookup: action.lookup}
        }
        case 'SET_ACTIVE_TOOLBAR':
        {
            return {...state, activeToolbar: action.activeToolbar}
        }
        case 'SET_TOOLBAR_ITEMS_ENABLED':
        {
            let toolbars = _.clone(state.toolbars);
            let toolbar = toolbars[action.toolbarId];
            if (toolbar === undefined)
            {
                console.error('state update with invalid toolbar id, SET_TOOLBAR_ITEMS_ENABLED');
                return state;
            }

            toolbar.items = toolbar.items.map(item => 
            {
                const disabled = action.itemIds.findIndex(it => it === item.id) !== -1 ? !action.enabled : undefined;
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
            let toolbars = _.clone(state.toolbars);
            let toolbar = toolbars[action.toolbarId];
            if (toolbar === undefined)
            {
                console.error('state update with invalid toolbar id, SET_TOOLBAR_ITEM_RUNNING');
                return state;
            }

            let item = toolbar.items.find(item => item.id === action.itemId);
            if (item === undefined)
            {
                console.error('state update with invalid toolbar-item id, SET_TOOLBAR_ITEM_RUNNING');
                return state;
            }

            item.running = action.running;

            return {
                ...state,
                toolbars: toolbars
            };
        }
        default:
            return state;
    }
}