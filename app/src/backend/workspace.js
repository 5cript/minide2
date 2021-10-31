import ApiBase from './apibase'

import sha256 from 'crypto-js/sha256';
import CryptoJS from 'crypto-js';
import * as Base64 from 'js-base64';

import {
    setFileTreeBranch,
    openWorkspace
} from '../actions/workspace_actions';

class Workspace extends ApiBase
{
    constructor(store, errorCallback, writeMessage)
    {
        super(store, writeMessage);
        this.errorCallback = errorCallback;
    }

    openWorkspace = async (path) =>
    {
        return this.writeMessage("/api/workspace/open", {
            "id": this.dataId,
            "path": path
        }).then(async (response) => {
            this.store.dispatch(setFileTreeBranch({
                directory: response.directory.origin,
                directories: response.directory.directories,
                files: response.directory.files
            }))
            return response;
        });
    }    

    saveFile = async (path, content) =>
    {
        return this.writeMessage("/api/workspace/saveFile", {
            path: path,
            sha256: sha256(content).toString(CryptoJS.enc.Hex),
            data: Base64.encode(content)
        });
    }

    enumDirectory = async (path) =>
    {
        return this.writeMessage("/api/workspace/enlist", {
            path: path
        }).then(async (response) => {
            console.log(response);
            this.store.dispatch(setFileTreeBranch({
                directory: response.directory.origin,
                directories: response.directory.directories,
                files: response.directory.files
            }))            
            return response;
        });
    }

    createFile = async (path, onFailure) =>
    {
        return this.saveFile(path, '').then(() => {
            return this.loadFile(path, undefined);
        })
    }

    loadFile = async (path, optionalFlag) =>
    {
        const flag = (optionalFlag !== undefined && optionalFlag !== null && optionalFlag !== '')
            ? optionalFlag
            : undefined
        ;
        return this.writeMessage("/api/workspace/loadFile", {
            path: path,
            flag: flag
        });
    }

    deleteFile = async (path) =>
    {
        return this.writeMessage("/api/workspace/deleteFile", {
            path: path
        });
    }

    loadProjectMetafile = async () =>
    {
        return this.writeMessage("/api/workspace/loadProjectMeta");
    }

    injectProjectSettings = async (settings) =>
    {
        return this.writeMessage("/api/workspace/changeProjectMeta", {
            settings: settings
        })
    }

    toggleSourceHeader = async (path, optionalFlag) =>
    {
        const flag = (optionalFlag !== undefined && optionalFlag !== null && optionalFlag !== '')
            ? optionalFlag
            : undefined
        ;
        return this.writeMessage("/api/workspace/toggleSourceHeader", {
            path: path,
            flag: flag
        });
    }

    setActiveProject = async (path) =>
    {
        return this.writeMessage("/api/workspace/setActiveProject", {
            path: path
        });
    }

    loadRunConfig = async () =>
    {
        return this.writeMessage("/api/workspace/getRunConfigs");
    }
}

export default Workspace;