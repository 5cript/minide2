/*
const loadDirectory = (root) => {
    return new Promise((resolve, reject) => {
        fs.readdir(root, (err, nodes) => {
            nodes.map(node => {
                return {
                    name: pathModifier.concat(root, node),
                    children: []
                }
            })
        })
    });
}
*/

/**
 * Adds items to the file tree
 */
export function setFileTreeBranch(directory, files, directories)
{
    return {
        type: 'SET_FILE_TREE_BRANCH',
        origin: directory,
        files: files,
        directories: directories
    };
}