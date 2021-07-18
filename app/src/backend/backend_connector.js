import WorkspaceApi from './workspace';
import ToolbarApi from './toolbar';
//import FetchImplementation from './implementations/fetch_impl';
//import NetTcpImplementation from './implementations/net_tcp_impl';
import WebSocketImplementation from './implementations/ws_impl';
import DebuggerRouter from './debugger';
import Router from './router';
import {
    setSessionId
} from '../actions/backend_actions';

class Backend extends Router
{
    constructor(store, controlCallback, dataCallback, errorCallback, onConnectionLoss)
    {
        super(store);

        this.workspaceRoutes = new WorkspaceApi(store, errorCallback);
        this.toolbarRoutes = new ToolbarApi(store, errorCallback);
        this.debuggerRoutes = new DebuggerRouter(store, errorCallback);

        this.routers = [
            this.workspaceRoutes,
            this.toolbarRoutes,
            this.debuggerRoutes
        ];

        //this.impl = new FetchImplementation(store, controlCallback, dataCallback, errorCallback, onConnectionLoss);
        //this.impl = new NetTcpImplementation(store, controlCallback, dataCallback, errorCallback, onConnectionLoss);
        this.impl = new WebSocketImplementation(store, controlCallback, dataCallback, errorCallback, onConnectionLoss);
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

    authenticate(continuation) 
    {
        let url = this.url("/api/authenticate");
        this.authFetch(url).then(async (res) => {
            const response = await res.json();
            this.store.dispatch(setSessionId(response.aSID));
            if (res.status < 300)
                continuation();
        })
    }

    readControl()
    {
        this.impl.readControl();
    }

    readData()
    {
        this.impl.readData();
    }
}

export default Backend;