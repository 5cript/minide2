import Router from './router'

import sha256 from 'crypto-js/sha256';
import CryptoJS from 'crypto-js';

class Workspace extends Router
{
    constructor(state, errorCallback)
    {
        super(state);
        this.errorCallback = errorCallback;
    }

    openWorkspace(path, onSuccess)
    {
        let url = this.url("/api/workspace/open");
        this.authFetch(
            url, 
            {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Accept': 'application/json'
                },
                body: JSON.stringify({
                    "id": this.dataId,
                    "path": path
                })
            }
        ).then(res => {
            if (res.status >= 300) {
                res.text().then((value) => {
                    this.errorCallback(value);
                });
                return;
            }
            if (onSuccess)
                onSuccess(path);
        });
    }    

    saveFile(path, content, onSuccess)
    {
        let hash = sha256(content);
        let json = JSON.stringify({path: path, sha256: hash.toString(CryptoJS.enc.Hex)});
        let jsonLen = json.length.toString(16).toUpperCase();
        jsonLen = "0x" + ("0000000" + jsonLen).slice(-8);
        let payload = jsonLen + '|' + json + content;
        
        let url = this.url("/api/workspace/saveFile");
        this.authFetch(
            url,
            {
                method: 'PUT',
                body: payload
            }
        ).then(res => {
            if (res.status >= 300) {
                res.text().then((value) => {
                    this.errorCallback(value);
                });
                return;
            }
            if (res.status === 200 || res.status === 204)
                onSuccess();
        });
    }

    enumDirectory(path)
    {
        this.postJson(this.url("/api/workspace/enlist"), {
            path: path
        });
    }

    createFile(path, onFailure)
    {
        this.saveFile(path, '', () => {
            this.loadFile(path, undefined, fail => {
                if (onFailure)
                    onFailure(this.tryParseJson(fail));
            });
        })
    }

    loadFile(path, optionalFlag, onFailure)
    {
        const flag = (optionalFlag !== undefined && optionalFlag !== null && optionalFlag !== '')
            ? optionalFlag
            : undefined
        ;
        this.postJson(
            this.url("/api/workspace/loadFile"), 
            {
                path: path,
                flag: flag
            }, 
            ()=>{}, 
            fail => {
                // assuming json, if it looks like json
                if (onFailure)
                    onFailure(this.tryParseJson(fail));
            }
        );
    }

    deleteFile(path)
    {
        this.postJson(this.url("/api/workspace/deleteFile"), {
            path: path
        });
    }

    loadProjectMetafile(onSuccess, onFailure)
    {
        this.get
        (
            this.url("/api/workspace/loadProjectMeta"), 
            (res) => 
            {
                if (onSuccess)
                    res.json().then(json => {
                        onSuccess(json);
                    }).catch(fail => {
                        onFailure(fail);
                    });
            },
            (failInfo) => 
            {
                if (onFailure)
                    onFailure(failInfo);
            }
        )
    }

    injectProjectSettings(settings)
    {
        this.postJson(this.url("/api/workspace/changeProjectMeta"), settings);
    }

    toggleSourceHeader(path, optionalFlag)
    {
        const flag = (optionalFlag !== undefined && optionalFlag !== null && optionalFlag !== '')
            ? optionalFlag
            : undefined
        ;
        this.postJson(this.url("/api/workspace/toggleSourceHeader"), {
            path: path,
            flag: flag
        });
    }

    setActiveProject(path, onSuccess)
    {
        this.postJson(this.url("/api/workspace/setActiveProject"), {
            path: path
        }, onSuccess);
    }

}

export default Workspace;