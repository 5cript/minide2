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