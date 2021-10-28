import WorkspaceApi from './workspace';
import ToolbarApi from './toolbar';
//import FetchImplementation from './implementations/fetch_impl';
//import NetTcpImplementation from './implementations/net_tcp_impl';
import WebSocketImplementation from './implementations/ws_impl';
import DebuggerRouter from './debugger';
import ApiBase from './apibase';
import {
    setSessionId
} from '../actions/backend_actions';

class Backend extends ApiBase
{
    constructor(store, onMessage, onConnectionLoss, onError)
    {
        super(store);

        this.workspaceRoutes = new WorkspaceApi(store, onError, this.writeMessage);
        this.toolbarRoutes = new ToolbarApi(store, onError, this.writeMessage);
        this.debuggerRoutes = new DebuggerRouter(store, onError, this.writeMessage);
        
        this.routers = [
            this.workspaceRoutes,
            this.toolbarRoutes,
            this.debuggerRoutes
        ];

        this.impl = new WebSocketImplementation(store, onMessage, onConnectionLoss, onError);
    }

    writeMessage = async (type, payload) =>
    {
        this.impl.writeMessage(type, payload);
    }

    workspace()
    {
        return this.workspaceRoutes;
    }

    toolbar()
    {
        return this.toolbarRoutes;
    }

    debugger()
    {
        return this.debuggerRoutes;
    }

    authenticate = async () =>
    {
        return new Promise((y,_) => y());
        // let url = this.url("/api/authenticate");
        // this.authFetch(url).then(async (res) => {
        //     const response = await res.json();
        //     this.store.dispatch(setSessionId(response.aSID));
        //     if (res.status < 300)
        //         continuation();
        // })
    }

    connect = async () => 
    {
        return this.impl.connect();
    }
}

export default Backend;