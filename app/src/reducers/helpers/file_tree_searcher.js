/**
 * on exact match returns {index: 'exact match index', match: true}.
 * on non exact match returns {index: 'where to insert, 0 equals front, 1 second etc', match: false}
 */
let binaryChildSearch = (node, what) => 
{
    let lexicographically_before = (x, y) => {
        return x.localeCompare(y) < 0;
    }

    if (node.children.length === 0)
        return {
            index: 0,
            match: false
        }

    let start = 0;
    let end = node.typeSplitIndex;
    if (end === 0) // end === 0 => action.directories.length === 0 => there is no directory => insert at front
        return {
            index: 0,
            match: false
        };

    if (end === 1)
        return {
            index: lexicographically_before(node.children[0].title, what) ? 1 : 0,
            match: (node.children[0].title === what)
        };
    
    // starting pivot:
    let pivot = ~~((end - start) / 2);    

    while (end - start > 1) 
    {
        let inList = node.children[pivot].title;    
        let lexicallyCompared = what.localeCompare(inList);
        if (lexicallyCompared === 0)
        {
            return {
                index: pivot,
                match: true
            };
        }
        // 'what' is left of pivot:
        if (lexicallyCompared < 0) 
        {
            end = pivot;
            pivot = ~~((pivot + start) / 2);
        }
        // 'what' is right of pivot:
        else
        {
            start = pivot;
            pivot = ~~((end + pivot) / 2);
        }
    }

    if (start === end) 
    {
        return {
            index: start,
            match: node.children[pivot].title === what
        };
    } 
    else 
    {
        let lexicallyCompared = what.localeCompare(node.children[start].title);
        if (lexicallyCompared === 0)
            return {
                index: start,
                match: true
            }
        if (lexicallyCompared < 0)
            return {
                index: start,
                match: false
            };
        else
            return {
                index: end,
                match: false
            };
    }
}

export default binaryChildSearch;