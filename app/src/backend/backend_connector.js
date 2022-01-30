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
    constructor(store, persistence, onMessage, onConnectionLoss, onError)
    {
        super(store);

        this.impl = new WebSocketImplementation({store, onMessage, onConnectionLoss, onError});

        this.workspaceRoutes = new WorkspaceApi({store, persistence, onError, impl: this.impl});
        this.toolbarRoutes = new ToolbarApi({store, persistence, onError, impl: this.impl});
        this.debuggerRoutes = new DebuggerRouter({store, persistence, onError, impl: this.impl});
        
        this.routers = [
            this.workspaceRoutes,
            this.toolbarRoutes,
            this.debuggerRoutes
        ];
    }

    authenticate = async () =>
    {
        return this.impl.authenticate();
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

    connect = async () => 
    {
        return this.impl.connect();
    }
}

export default Backend;