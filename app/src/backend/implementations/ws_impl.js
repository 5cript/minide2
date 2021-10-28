import Router from '../apibase';

class WebSocketImplementation
{
    constructor(store, onMessage, onConnectionLoss, onError)
    {
        this.store = store;
        this.onMessage = onMessage;
        this.onConnectionLoss = onConnectionLoss;
        this.onError = onError;
        this.replyId = 0;
        this.replyWaiter = {};
        
        this.buffer = '';
    }
    
    writeMessage = async (type, payload) =>
    {
        const replyId = this.replyId++;
        if (replyId > 1000000)
            this.replyId = 0;
        return new Promise((fullfil, reject) => {
            this.replyWaiter[replyId] = {
                fullfil: fullfil,
                reject: reject
            };
            this.wsClient.send(JSON.stringify({
                type: type,
                payload: payload,
                replyId: replyId
            }));
        });
    }

    sendInitialInformation = () =>
    {
        const backend = this.store.getState().backend;
        this.writeMessage("authentication", {
            sessionId: backend.sessionId,
            user: "admin",
            password: "dummy"
        })
    }

    wsUrl = (url, control) =>
    {
        let state = this.store.getState();
        return "ws://" + state.backend.ip + ":" + state.backend.websocketPort + url;
    }

    parseData = () =>
    {
        let size = parseInt(this.buffer.substr(0, 10));
        if (isNaN(size)) {
            console.error('oh big no! expected number in stream');
            return;
        }
        if (this.buffer.length >= size + 12) {
            try {
                let json = JSON.parse(this.buffer.substr(11, size));
                this.buffer = this.buffer.slice(12 + size);
                const reply = this.replyWaiter[json.replyId];
                if (reply !== undefined) {
                    reply.fullfil(json);
                    delete this.replyWaiter[json.replyId];
                }
                else
                    this.onMessage(json);
            }
            catch(e) {
                console.error("error in stream reader",e);
            }
        }
    }
    
    connect = async () =>
    {
        return new Promise((resolve, _) => {
            this.wsClient = new WebSocket(this.wsUrl("/api/wsstreamer/data", true));
            this.wsClient.onopen = () => {
                console.log("ws is open");
                this.sendInitialInformation();
                resolve();
            }
            this.wsClient.onclose = () => {
                this.onConnectionLoss();
                this.wsClient = undefined;
            }
            this.wsClient.onerror = () => {
                this.onError();
            }
            this.wsClient.onmessage = (event) => {
                console.log("dataMessage", event);
                this.buffer += event.data;
                this.buffer = this.parseData();
            }
        })
    }
}

export default WebSocketImplementation;