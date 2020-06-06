import _ from 'lodash';
import binaryChildSearch from './helpers/file_tree_searcher';

const initialState = 
{
    root: '',
    fileTree: 
    {
        key: '0',
        title: '',
        typeSplitIndex: 0, // the position where the data goes from directories to files.
        children: []
    }
};

export default function reducer(state = initialState, action) 
{
    switch (action.type) 
    {
        case 'OPEN_WORKSPACE': 
        {
            return {...state, root: action.payload}
        }
        case 'SET_FILE_TREE_BRANCH': 
        {
            if (action.directories === undefined)
                action.directories = [];
            if (action.files === undefined)
                action.files = [];

            let lexiSortCompare = (lhs, rhs) => {
                return lhs.localeCompare(rhs);
            }

            action.directories.sort(lexiSortCompare);
            action.files.sort(lexiSortCompare);

            let originSplit = action.origin.split('/');
            originSplit.shift();
            
            let insertInTree = (node, index, title, key) => 
            {
                node.children.splice(index, 0, {
                    title: title, key: key, typeSplitIndex: 0
                });
                ++node.typeSplitIndex;
                return node.children[index];
            }

            let catenateOriginSplit = (depth) => 
            {
                let cat = '/';
                for (let i = 0; i !== depth; ++i)
                    cat += originSplit[i];
                return cat;
            }

            let setChildren = (node) => {
                node.children = [];
                for (let i of action.directories) 
                    node.children.push({key: action.origin + "/" + i, title: i, isLeaf: false});
                node.typeSplitIndex = action.directories.length;
                for (let i of action.files) 
                    node.children.push({key: action.origin + "/" + i, title: i});
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
                setChildren(res);
                res.key = action.origin;
                res.title = originSplit[0];
                return {...state, root: action.origin, fileTree: res};
            }
            return {...state, fileTree: res};
        }
        default:
            return state;
    }
}