import Router from './router'

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
        fetch(
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

    enumDirectory(path)
    {
        this.postJson(this.url("/api/workspace/enlist"), this.dataId, {
            path: path
        });
    }

    loadFile(path)
    {
        this.postJson(this.url("/api/workspace/loadFile"), this.dataId, {
            path: path
        });
    }

}

export default Workspace;