import Router from '../apibase';
import {DateTime} from 'luxon';
import {SHA256} from 'crypto-js';
import {generateUuid} from '../../util/random_id';

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
    constructor({store, onMessage, onConnectionLoss, onError})
    {
        const replyTimeoutSeconds = 30;
        const replyTimeoutRefreshMilliseconds = 3000;

        this.store = store;
        this.onMessage = onMessage;
        this.onConnectionLoss = onConnectionLoss;
        this.onError = onError;
        this.replyId = 0;
        this.replyWaiter = {};
        this.eventHooks = {};
        
        this.textBuffer = '';
        setInterval(() => {
            for (const i in this.replyWaiter) 
            {
                const diff = DateTime.now().diff(this.replyWaiter[i].date, ['seconds']).seconds;
                if (diff > replyTimeoutSeconds)
                {
                    this.onError("no reply recieved for " + this.replyWaiter[i].type);
                    this.replyWaiter[i].responsePromise.reject();
                    delete this.replyWaiter[i];
                }
            }
        }, replyTimeoutRefreshMilliseconds);
    }

    registerEventListener = (eventName, onEvent) => {
        if (!this.eventHooks.hasOwnProperty(eventName))
            this.eventHooks[eventName] = []

        const id = generateUuid();
        this.eventHooks[eventName].push({
            id,
            onEvent
        });
        return id;
    }

    removeEventListener = (eventName, id) => {
        if (!this.eventHooks.hasOwnProperty(eventName))
        {
            console.log('There is no hook for that event name.');
            return;
        }

        this.eventHooks[eventName] = this.eventHooks[eventName].filter(hook => hook.id !== id);
        if (this.eventHooks[eventName].length === 0)
            delete this.eventHooks[eventName];
    }
    
    writeMessage = async (type, payload, onBinary) =>
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
            date: DateTime.now(),
            onBinary: onBinary ? onBinary : ()=>{this.onError('receiving unexpected binary data')}
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
        let size = parseInt(this.textBuffer.substr(0, 10));
        if (isNaN(size)) {
            this.onError('oh big no! expected number in stream');
            return;
        }
        if (this.textBuffer.length >= size + 11) {
            try {
                let json = JSON.parse(this.textBuffer.substr(11, size));
                this.textBuffer = this.textBuffer.slice(11 + size);
                const reply = this.replyWaiter[json.ref];
                if (reply !== undefined) {
                    if (json.error)
                        reply.responsePromise.reject(json.error);
                    else
                        reply.responsePromise.resolve(json);
                    delete this.replyWaiter[json.ref];
                    return;
                }
                console.log('onMessage', json);
                const hooks = this.eventHooks[json.event];
                if (hooks !== undefined) {
                    hooks.forEach((hook) => {
                        hook.onEvent(json);
                    })
                    return;
                }
                else
                {
                    this.onMessage(json);
                }
            }
            catch(e) {
                this.onError("error in stream reader",e);
            }
        }
    }

    handleBinaryCompletion = (ref) => 
    {
        console.log('file complete for ', ref);
        let repl = this.replyWaiter[ref];
        repl.responsePromise.resolve(repl.binaryBuffer);
        delete this.replyWaiter[ref];
    }
    
    connect = async () =>
    {
        return new Promise((resolve, _) => {
            this.wsClient = new WebSocket(this.wsUrl("/api/wsstreamer/data", true));
            this.wsClient.binaryType = "arraybuffer";
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
                console.log("dataMessage received");
                if (event.data instanceof ArrayBuffer)
                {
                    var enc = new TextDecoder("utf-8");
                    const ref = parseInt(enc.decode(event.data.slice(0, 10)));
                    let repl = this.replyWaiter[ref];
                    if (repl)
                    {
                        if (event.data.byteLength != 10)
                        {
                            repl.onBinary(event.data.slice(10, event.data.byteLength));
                            repl.data = DateTime.now();
                        }
                        else
                        {
                            this.handleBinaryCompletion(ref);
                        }
                    }
                    return;
                }
                if (event && event.data)
                {
                    this.textBuffer += event.data;
                    this.parseData();
                }
            }
        })
    }
}

export default WebSocketImplementation;