import Router from '../apibase';
import {DateTime} from 'luxon';
import {SHA256} from 'crypto-js';

class ResponsePromise
{
    constructor() {
        this.promise = new Promise((resolve, reject) => {
            this.resolve = resolve;
            this.reject = reject;
        });
    }
}

class WebSocketImplementation
{
    constructor(store, onMessage, onConnectionLoss, onError)
    {
        const replyTimeoutSeconds = 30;
        const replyTimeoutRefreshMilliseconds = 3000;

        this.store = store;
        this.onMessage = onMessage;
        this.onConnectionLoss = onConnectionLoss;
        this.onError = onError;
        this.replyId = 0;
        this.replyWaiter = {};
        
        this.buffer = '';
        setInterval(() => {
            for (const i in this.replyWaiter) 
            {
                const diff = DateTime.now().diff(this.replyWaiter[i].date, ['seconds']).seconds;
                if (diff > replyTimeoutSeconds)
                {
                    console.error("no reply recieved for " + this.replyWaiter[i].type);
                    this.replyWaiter[i].responsePromise.reject();
                    delete this.replyWaiter[i];
                }
            }
        }, replyTimeoutRefreshMilliseconds);
    }
    
    writeMessage = async (type, payload) =>
    {
        if (this.wsClient === undefined)
            return;
        if (this.wsClient.readyState !== WebSocket.OPEN)
            return;

        const replyId = this.replyId++;
        if (replyId > 1000000)
            this.replyId = 0;
        this.replyWaiter[replyId] = {
            type: type,
            responsePromise: new ResponsePromise(),
            date: DateTime.now()
        };
        this.wsClient.send(JSON.stringify({
            type: type,
            payload: payload,
            ref: replyId
        }));
        return this.replyWaiter[replyId].responsePromise.promise;
    }

    authenticate = () =>
    {
        const backend = this.store.getState().backend;
        return this.writeMessage("/api/user/authenticate", {
            sessionId: backend.sessionId,
            user: "dummy",
            password: SHA256("dummy")
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
        if (this.buffer.length >= size + 11) {
            try {
                let json = JSON.parse(this.buffer.substr(11, size));
                this.buffer = this.buffer.slice(11 + size);
                const reply = this.replyWaiter[json.ref];
                if (reply !== undefined) {
                    if (json.error)
                        reply.responsePromise.reject(json.error);
                    else
                        reply.responsePromise.resolve(json);
                    delete this.replyWaiter[json.ref];
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
                if (event && event.data)
                {
                    this.buffer += event.data;
                    this.parseData();
                }
            }
        })
    }
}

export default WebSocketImplementation;