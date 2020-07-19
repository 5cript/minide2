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

export function setActiveProject(path)
{
    return {
        type: 'SET_ACTIVE_PROJECT',
        path: path
    };
}

export function setHoveredNode(path)
{
    return {
        type: 'SET_HOVERED_NODE',
        path: path
    }
}