import Router from '../router';

//const net = window.require('electron').remote.require('net');
const net = window.require('net');

class NetTcpImplementation
{
    constructor(store, controlCallback, dataCallback, errorCallback, onConnectionLoss)
    {
        console.log(net);

        this.store = store;
        this.controlCallback = controlCallback;
        this.dataCallback = dataCallback;
        this.errorCallback = errorCallback;
        this.onConnectionLoss = onConnectionLoss;
        
        this.controlBuffer = '';
        this.dataBuffer = '';
    }

    makeHeader(url)
    {        
        const state = this.store.getState();
        const ip = state.backend.ip;
        const port = state.backend.port;
        const asid = state.backend.sessionId;

        const header =
            "GET " + url + " HTTP/1.1\r\n" + 
            "Host: " + ip + ":" + port + "\r\n" +
            "Connection: keep-alive\r\n" +
            "User-Agent: Electron/11.0.0-beta.4\r\n" +
            "Accept: */*\r\n" +
            "Sec-Fetch-Site: cross-site\r\n" +
            "Sec-Fetch-Mode: cors\r\n" +
            "Sec-Fetch-Dest: empty\r\n" +
            "Accept-Encoding: gzip, deflate, br\r\n" +
            "Accept-Language: en-US\r\n" +
            "Cookie: aSID=" + asid + "\r\n\r\n"
        ;

        return header;
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
        const state = this.store.getState();
        const ip = state.backend.ip;
        const port = state.backend.port;
        
        this.controlSocket = new net.Socket();
        this.controlSocket.connect(
            port, 
            ip,
            () =>
            {
                console.log('Connected');
                this.controlSocket.write(this.makeHeader("/api/streamer/control"));
                //client.write('Hello, server! Love, Client.');
            }
        );
        
        this.controlSocket.on('data', (data) =>
        {
            console.log('Received: ' + data);

            if (data)
                this.controlBuffer += data;

            let size = parseInt(this.controlBuffer.substr(0, 10));
            if (isNaN(size)) 
            {
                this.errorCallback('oh big no! expected number in stream');
                return;
            }
            if (this.controlBuffer.length >= size + 12) 
            {
                try 
                {
                    let json = JSON.parse(this.controlBuffer.substr(11, size));
                    this.controlBuffer = this.controlBuffer.slice(12 + size);
                    this.handleControlMessage(json);
                }
                catch(e) 
                {
                    this.errorCallback(e);
                }
            }
            //controlSocket.destroy(); // kill client after server's response
        });
        
        this.controlSocket.on('close', () =>
        {
            console.log('Connection closed');
        });
    }

    close()
    {
        if (this.controlSocket)
            this.controlSocket.destroy();
        if (this.dataSocket)
            this.dataSocket.destroy();
    }

    readData()
    {
        const state = this.store.getState();
        const ip = state.backend.ip;
        const port = state.backend.port;

        this.dataSocket = new net.Socket();
        this.dataSocket.connect(
            port, 
            ip,
            () => 
            {
                console.log('Connected');

                this.dataSocket.write(this.makeHeader("/api/streamer/data"));
                //client.write('Hello, server! Love, Client.');
            }
        );
        
        this.dataSocket.on('data', (data) =>
        {
            console.log('Received: ' + data);

            if (data)
                this.dataBuffer += data;

            let size = parseInt(this.dataBuffer.substr(0, 10));
            if (isNaN(size)) 
            {
                this.errorCallback('oh big no! expected number in stream');
                return;
            }
            if (this.dataBuffer.length >= size + 12) 
            {
                try 
                {
                    let json = JSON.parse(this.dataBuffer.substr(11, size));
                    this.dataBuffer = this.dataBuffer.slice(12 + size);
                    this.handleDataMessage(json);
                }
                catch(e) 
                {
                    this.errorCallback(e);
                }
            }
        });
        
        this.dataSocket.on('close', () =>
        {
            console.log('Connection closed');
        });
    }
};

export default NetTcpImplementation;