import Workspace from './backend/workspace';
import Router from './backend/router';

class Backend extends Router
{
    constructor(store, controlCallback, dataCallback, errorCallback)
    {
        super(store);
        this.controlBuffer = '';
        this.dataBuffer = '';
        this.controlCallback = controlCallback;
        this.dataCallback = dataCallback;
        this.errorCallback = errorCallback;

        this.workspaceRoutes = new Workspace(store, errorCallback);
        this.routers = [
            this.workspaceRoutes
        ];

        if (!this.controlCallback) {
            this.controlCallback = (json) => {
                console.log("WARNING! please supply a control callback");
                console.log(json)
            }
        }
        if (!this.dataCallback) {
            this.dataCallback = (json) => {
                console.log("WARNING! please supply a data callback");
                console.log(json)
            }
        }
        if (!this.errorCallback) {
            this.errorCallback = (msg) => {
                console.error(msg);
            }
        }
    }

    workspace()
    {
        return this.workspaceRoutes;
    }

    attachToStreams()
    {
        this.readControl();
        this.readData();
    }

    handleControlMessage(json)
    {
        if (json.type === "welcome") {
            this.setControlId(json.id);
            for (let i in this.routers)
                this.routers[i].setControlId(this.controlId);
            return;
        }
        this.controlCallback(json);
    }

    handleDataMessage(json)
    {
        if (json.type === "welcome") {
            this.setDataId(json.id);
            for (let i in this.routers)
                this.routers[i].setDataId(this.dataId);
            return;
        }
        this.dataCallback(json);
    }

    readControl()
    {
        let url = this.url("/api/streamer/control");

        fetch(url).then((res) => {
            if (res.status >= 300) {
                res.text().then((value) => {
                    this.errorCallback(value);
                });
                return;
            }
            let reader = res.body.getReader();
            let decoder = new TextDecoder();
            let read = () => {
                return reader.read().then(({value, done}) => {
                    if (done) {
                        console.log('stream ended');
                        return;
                    }
                    if (value)
                        this.controlBuffer += decoder.decode(value, {stream: !done});
                    let size = parseInt(this.controlBuffer.substr(0, 10));
                    if (isNaN(size)) {
                        console.error('oh big no! expected number in stream');
                        return read();
                    }
                    if (this.controlBuffer.length >= size + 12) {
                        try {
                            let json = JSON.parse(this.controlBuffer.substr(11, size));
                            this.controlBuffer = this.controlBuffer.slice(12 + size);
                            this.handleControlMessage(json);
                        }
                        catch(e) {
                            console.error("oh no " + e);
                        }
                    }
                    
                    return read();
                });
            };
            return read();
        })
    }

    readData()
    {
        let url = this.url("/api/streamer/data");

        fetch(url).then((res) => {
            if (res.status >= 300) {
                res.text().then((value) => {
                    this.errorCallback(value);
                });
                return;
            }
            let reader = res.body.getReader();
            let decoder = new TextDecoder();
            let read = () => {
                return reader.read().then(({value, done}) => {
                    if (done) {
                        console.log('stream ended');
                        return;
                    }
                    if (value)
                        this.dataBuffer += decoder.decode(value, {stream: !done});
                    let size = parseInt(this.dataBuffer.substr(0, 10));
                    if (isNaN(size)) {
                        console.error('oh big no! expected number in stream');
                        return read();
                    }
                    if (this.dataBuffer.length >= size + 12) {
                        try {
                            let json = JSON.parse(this.dataBuffer.substr(11, size));
                            this.dataBuffer = this.dataBuffer.slice(12 + size);
                            this.handleDataMessage(json);
                        }
                        catch(e) {
                            console.error("oh no " + e);
                            console.log(e.stack);
                        }
                    }
                    
                    return read();
                });
            };
            return read();
        });
    }
}

export default Backend;