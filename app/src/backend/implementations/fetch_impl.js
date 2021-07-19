import Router from '../router';

class FetchImplementation extends Router
{
    constructor(store, controlCallback, dataCallback, errorCallback, onConnectionLoss)
    {
        super(store);
        this.controlCallback = controlCallback;
        this.dataCallback = dataCallback;
        this.errorCallback = errorCallback;
        this.onConnectionLoss = onConnectionLoss;
        
        this.controlBuffer = '';
        this.dataBuffer = '';
    }

    handleControlMessage(json)
    {
        if (json.type === "welcome") {
            this.setControlId(json.id);
            for (let i in this.routers)
                this.routers[i].setControlId(this.controlId);
        }
        this.controlCallback(json);
    }

    handleDataMessage(json)
    {
        if (json.type === "welcome") {
            this.setDataId(json.id);
            for (let i in this.routers)
                this.routers[i].setDataId(this.dataId);
        }
        this.dataCallback(json);
    }

    readControl()
    {
        let url = this.url("/api/streamer/control");

        this.authFetch(url).then((res) => {
            if (res.status >= 300) {
                res.text().then((value) => {
                    this.errorCallback(value);
                });
                return;
            }
            let reader = res.body.getReader();
            let decoder = new TextDecoder();
            let read = () => 
            {
                return reader.read().then(({value, done}) => 
                {
                    if (done) {
                        this.onConnectionLoss('control');
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
                            console.error("error in control stream reader",e);
                        }
                    }
                    
                    //console.log('readNext');
                    return read();
                }).catch(reject => {
                    this.onConnectionLoss('control_error: ' + reject.message);
                })
            };
            return read();
        }).catch(reason => 
        {
            this.onConnectionLoss('control_error');
        });
    }

    readData()
    {
        let url = this.url("/api/streamer/data");

        this.authFetch(url).then((res) => {
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
                        this.onConnectionLoss('data');
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
                            console.error("error in data stream reader",e);
                            console.error(e.stack);
                        }
                    }
                    
                    return read();
                })
            };
            return read();
        }).catch(reason => 
        {
            this.onConnectionLoss('data_error');
        });;
    }
}

export default FetchImplementation;