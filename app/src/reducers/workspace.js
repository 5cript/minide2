const _ = require('lodash');
const binaryChildSearch = require('./helpers/file_tree_searcher');

const initialState = 
{
    root: '',
    fileTree: 
    {
        key: '0',
        title: '',
        typeSplitIndex: 0, // the position where the data goes from directories to files.
        children: []
    },
    activeProject: undefined,
    hoveredNode: undefined
};

const findNodeAndSetStyle = (path, topNode, style, includeFiles) =>
{
    let originSplit = path.split('/');
    originSplit.shift();

    let curNode = topNode;
    for (let depth = 1; depth !== originSplit.length; ++depth) 
    {
        let searchResult = binaryChildSearch(curNode, originSplit[depth]);
        if (searchResult.match === false && includeFiles)
        {
            searchResult = binaryChildSearch(curNode, originSplit[depth], true);
        }
        if (searchResult.match === false) 
            console.error('path not found in tree', curNode, originSplit[depth]);
        else 
            curNode = curNode.children[searchResult.index];
    }
    curNode.style = style;
};

module.exports = function reducer(state = initialState, action) 
{
    switch (action.type) 
    {
        case 'OPEN_WORKSPACE': 
        {
            return {...state, root: action.payload.path}
        }
        case 'SET_HOVERED_NODE': 
        {
            let res = _.cloneDeep(state.fileTree);

            if (state.hoveredNode !== undefined)
            {
                if (state.hoveredNode !== state.activeProject)
                    findNodeAndSetStyle(state.hoveredNode, res, undefined, true);
                else
                    findNodeAndSetStyle(state.hoveredNode, res, {
                        fontWeight: 'bold',
                        color: 'var(--theme-color)'
                    });
            }
            
            if (action.payload.path !== undefined)
            {
                if (action.payload.path === state.activeProject)
                    findNodeAndSetStyle(action.payload.path, res, {
                        backgroundColor: 'var(--background-color-brighter)',
                        fontWeight: 'bold',
                        color: 'var(--theme-color)'
                    }, true)
                else
                    findNodeAndSetStyle(action.payload.path, res, {
                        backgroundColor: 'var(--background-color-brighter)'
                    }, true)
            }

            return {...state, fileTree: res, hoveredNode: action.payload.path}; 
        }
        case 'SET_ACTIVE_PROJECT':
        {
            let res = _.cloneDeep(state.fileTree);

            if (state.activeProject !== undefined)
                findNodeAndSetStyle(state.activeProject, res, undefined);

            findNodeAndSetStyle(action.payload.path, res, {
                fontWeight: 'bold',
                color: 'var(--theme-color)'
            });

            return {...state, activeProject: action.payload.path, fileTree: res};   
        }
        case 'SET_FILE_TREE_BRANCH': 
        {
            if (action.payload.directories === undefined)
                action.payload.directories = [];
            if (action.payload.files === undefined)
                action.payload.files = [];

            const lexiSortCompare = (lhs, rhs) => {
                return lhs.localeCompare(rhs);
            }

            action.payload.directories.sort(lexiSortCompare);
            action.payload.files.sort(lexiSortCompare);

            let originSplit = action.payload.origin.split('/');
            originSplit.shift();
            
            const insertInTree = (node, index, title, key) => 
            {
                node.children.splice(index, 0, {
                    title: title, key: key, typeSplitIndex: 0
                });
                ++node.typeSplitIndex;
                return node.children[index];
            }

            const catenateOriginSplit = (depth) => 
            {
                let cat = '/';
                for (let i = 0; i !== depth; ++i)
                    cat += originSplit[i];
                return cat;
            }

            const setChildren = (node) => 
            {
                node.children = [];
                for (let i of action.payload.directories) 
                    node.children.push({key: action.payload.origin + "/" + i, title: i, isLeaf: false});
                node.typeSplitIndex = action.payload.directories.length;
                for (let i of action.payload.files) 
                {
                    if (i[0] !== '.')
                        node.children.push({key: action.payload.origin + "/" + i, title: i});
                    else
                        node.children.push({key: action.payload.origin + "/" + i, title: i, style: {
                            color: 'var(--invisible-file)'
                        }});
                }
            }

            // is not root?
            let res = _.cloneDeep(state.fileTree);
            if (originSplit.length > 1) 
            {
                //res = {};                
                let curNode = res;
                if (curNode.title !== originSplit[0]) 
                    console.error('root of branch set operation does not match');

                for (let depth = 1; depth !== originSplit.length; ++depth) 
                {
                    let searchResult = binaryChildSearch(curNode, originSplit[depth]);
                    if (searchResult.match === false) 
                        curNode = insertInTree(curNode, searchResult.index, originSplit[depth], catenateOriginSplit(depth));
                    else 
                        curNode = curNode.children[searchResult.index];
                }
                setChildren(curNode);
            } 
            else 
            {            
                res = _.cloneDeep(initialState.fileTree);    
                res.isLeaf = false;
                setChildren(res);
                res.key = action.payload.origin;
                res.title = originSplit[0];
                return {...state, root: action.payload.origin, fileTree: res};
            }
            return {...state, fileTree: res};
        }
        default:
            return state;
    }
}