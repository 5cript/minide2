import Router from '../router';

class WebSocketImplementation extends Router
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

    sendInitialInformation(socket)
    {
        const backend = this.store.getState().backend;
        socket.send(JSON.stringify({
            type: "authentication",
            sessionId: backend.sessionId,
            user: "admin",
            password: "dummy"
        }));
    }
    wsUrl(url, control)
    {
        let state = this.store.getState();
        return ("ws://" + state.backend.ip + ":" + (control ? state.backend.controlPort : state.backend.dataPort)) + url;
    }
    parseData(data, onMessage)
    {
        let size = parseInt(data.substr(0, 10));
        if (isNaN(size)) {
            console.error('oh big no! expected number in stream');
            return;
        }
        if (data.length >= size + 12) {
            try {
                let json = JSON.parse(data.substr(11, size));
                data = data.slice(12 + size);
                onMessage(json);
            }
            catch(e) {
                console.error("error in control stream reader",e);
            }
        }
        return data;
    }
    readControl()
    {
        this.controlSocket = new WebSocket(this.wsUrl("/api/wsstreamer/control", true));
        this.controlSocket.onopen = () => {
            this.sendInitialInformation(this.controlSocket);
        }
        this.controlSocket.onclose = () => {
            this.onConnectionLoss();
            this.controlSocket = undefined;
        }
        this.controlSocket.onerror = () => {
            this.errorCallback();
        }
        this.controlSocket.onmessage = (event) => {
            this.controlBuffer += event.data;
            this.controlBuffer = this.parseData(this.controlBuffer, this.controlCallback);
        }
    }
    readData()
    {
        this.dataSocket = new WebSocket(this.wsUrl("/api/wsstreamer/data", true));
        this.dataSocket.onopen = () => {
            console.log("dataOpen");
            this.sendInitialInformation(this.dataSocket);
        }
        this.dataSocket.onclose = () => {
            this.onConnectionLoss();
            this.dataSocket = undefined;
        }
        this.dataSocket.onerror = () => {
            this.errorCallback();
        }
        this.dataSocket.onmessage = (event) => {
            console.log("dataMessage", event);
            this.dataBuffer += event.data;
            this.dataBuffer = this.parseData(this.dataBuffer, this.dataCallback);
        }
    }
}

export default WebSocketImplementation;