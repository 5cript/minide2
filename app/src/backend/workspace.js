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

    openWorkspace(path)
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

    loadFile(path, optionalFlag)
    {
        const flag = (optionalFlag !== undefined && optionalFlag !== null && optionalFlag !== '')
            ? optionalFlag
            : undefined
        ;
        this.postJson(this.url("/api/workspace/loadFile"), {
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