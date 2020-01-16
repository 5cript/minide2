import { pathModifier } from '../util/path_util';

const fs = window.require('fs');

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

export function openWorkspace(directory) {
    return {
        type: 'OPEN_WORKSPACE',
        payload: {
            directory: directory,
            tree: loadDirectory(directory)
        }
    }
}