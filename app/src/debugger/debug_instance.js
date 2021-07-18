import {
    debuggerConsoleStream, 
    debuggerAddLibrary,
    debuggerThreadExit,
    debuggerThreadCreated,
    debuggerSetProcessLife,
    debuggerSetAlive,
    debuggerSetProcessExitCode
} from '../actions/debugging_actions';
import {unpackConstant, dedash} from './value_unpacker';

import _ from 'lodash';

/**
 * Commands to execute at startup:
 * 
 * 1) set new-console on
 */

class DebugInstance
{
    constructor(backend, store, instanceId)
    {
        this.backend = backend;
        this.debuggerBackend = this.backend.debugger();
        this.workspaceBackend = this.backend.workspace();
        this.store = store;
        this.instanceId = instanceId;

        this.verbose = true;
    }

    onConsoleStream(data)
    {
        this.printToConsole(data);
    }

    onDebuggerExit(info)
    {
        this.store.dispatch(debuggerSetAlive(this.instanceId, false));
    }

    printToConsole(data)
    {
        this.store.dispatch(debuggerConsoleStream(this.instanceId, data))
    }

    printVerbose(data)
    {
        if (this.verbose)
        {
            this.printToConsole(data);
        }
    }

    onLogStream(data)
    {
        this.printToConsole("->" + data);
    }

    onNotifyRecord(record)
    {
        let resultsObject = this.recordResultsToObject(record.results);
        switch (record.status)
        {
            case('thread-group-added'):
            {
                this.onThreadGroupAdded(resultsObject);
                break;
            }
            case('thread-group-started'):
            {
                this.onThreadGroupStarted(resultsObject);
                break;
            }
            case('thread-group-exited'):
            {
                this.onThreadGroupExit(resultsObject);
                break;
            }
            case('thread-created'):
            {
                this.onThreadCreated(resultsObject);
                break;
            }
            case('library-loaded'):
            {
                this.onLibraryLoaded(resultsObject);
                break;
            }
            case('thread-exited'):
            {
                this.onThreadExit(resultsObject);
                break;
            }
            default:
                console.log(record);
        }
    }

    onThreadGroupExit(resultsObject)
    {
        this.store.dispatch(debuggerSetProcessExitCode(this.instanceId, resultsObject.exitCode));
    }

    onExecRecord(record)
    {
        let resultsObject = this.recordResultsToObject(record.results);
        switch(record.status)
        {
            case('stopped'):
            {
                this.onProgramStop(resultsObject);
                break;
            }
            default:
                console.log(record);
        }
    }

    onProgramStop(stopInfo)
    {
        this.store.dispatch(debuggerSetProcessLife(this.instanceId, false));
    }

    onThreadCreated(thread)
    {
        this.store.dispatch(debuggerThreadCreated(this.instanceId, thread));
    }

    onThreadExit(thread)
    {
        this.store.dispatch(debuggerThreadExit(this.instanceId, thread.id));
    }

    recordResultsToObject(results)
    {
        let object = {}
        results.forEach(result => {
            if (_.isObject(result.value))
                object[dedash(result.variable)] = result.value.value;
            else
                object[dedash(result.variable)] = result.value;
        })
        return object;
    }

    onLibraryLoaded(library)
    {
        this.printVerbose("{Library Loaded: " + library.hostName + "}\n");
        this.store.dispatch(debuggerAddLibrary(this.instanceId, library));        
    }

    onThreadGroupAdded(values)
    {
        //console.log(values)
        let msg = "{Thread Group Added - ID: ";
        msg += unpackConstant(values.id); // its stupidly packed.
        msg += "}\n";
        this.printVerbose(msg);
    }

    onThreadGroupStarted(values)
    {
        //console.log(values)
        let msg = "{Thread Group Started - ID: ";
        msg += unpackConstant(values.id); // its stupidly packed.
        msg += "}\n";
        this.printVerbose(msg);
    }
}

export default DebugInstance;