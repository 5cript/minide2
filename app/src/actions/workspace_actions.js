/**
 * Adds items to the file tree
 */
export function setFileTreeBranch({directory, files, directories})
{
    return {
        type: 'SET_FILE_TREE_BRANCH',
        payload: {
            origin: directory,
            files: files,
            directories: directories
        }
    };
}

export function setActiveProject(path)
{
    return {
        type: 'SET_ACTIVE_PROJECT',
        payload: {
            path: path
        }
    };
}

export function setHoveredNode(path)
{
    return {
        type: 'SET_HOVERED_NODE',
        payload: {
            path: path
        }
    }
}

export function openWorkspace({path})
{
    return {
        type: 'OPEN_WORKSPACE',
        payload: {
            path: path
        }
    }
}

export function removeFileTreeElement(path)
{
    return {
        type: 'DELETE_WORKSPACE_ELEMENT',
        payload: {
            path: path
        }
    }
}