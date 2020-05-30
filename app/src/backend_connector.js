class Backend
{
    constructor(store, controlCallback, errorCallback)
    {
        this.store = store;
        this.controlBuffer = '';
        this.controlCallback = controlCallback;
        this.errorCallback = errorCallback;
        if (!this.controlCallback) {
            this.controlCallback = (json) => {
                console.log("WARNING! please supply a control callback");
                console.log(json)
            }
        }
        if (!this.errorCallback) {
            this.errorCallback = (msg) => {
                console.error(msg);
            }
        }
    }

    _getHost()
    {
        let state = this.store.getState();
        return "http://" + state.backend.ip + ":" + state.backend.port;
    }

    attachToStreams()
    {
        this.readControl();
    }

    readControl()
    {
        let url = this._getHost() + "/api/streamer/control";

        fetch(url).then((res) => {
            if (res.status >= 300) {
                res.text().then((value) => {
                    this.errorCallback(value);
                });
                return;
            }
            let reader = res.body.getReader();
            let decoder = new TextDecoder();
            let readData = () => {
                return reader.read().then(({value, done}) => {
                    if (done) {
                        console.log('stream ended');
                        return;
                    }
                    if (value)
                        this.controlBuffer += decoder.decode(value, {stream: !done});
                    let size = parseInt(this.controlBuffer.substr(0, 10));
                    if (size === NaN) {
                        console.error('oh big no! expected number in stream');
                        return readData();
                    }
                    if (this.controlBuffer.length >= size + 12) {
                        try {
                            let json = JSON.parse(this.controlBuffer.substr(11, size));
                            this.controlBuffer = this.controlBuffer.slice(12 + size);
                            this.controlCallback(json);
                        }
                        catch(e) {
                            console.error("oh no " + e);
                        }
                    }
                    
                    return readData();
                });
            };
            return readData();
        })
    }
}

export default Backend;