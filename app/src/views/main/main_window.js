import React from 'react';

// Components
import Toolbar from './components/toolbar';
//import FileView from './components/file_tree';
import Explorer from './components/explorer';
import SplitterLayout from 'react-splitter-layout';
import Editor from './components/editor';
import LogsAndOthers from './components/logs_and_term';
import {connect} from 'react-redux';

// Actions
import {setFileTreeBranch} from '../../actions/workspace_actions';
import {addOpenFileWithContent} from '../../actions/open_file_actions';

// Other
import Backend from '../../backend_connector';

// Style
import './styles/main.css'

// requires
const {ipcRenderer} = window.require('electron');
const Classes = window.require('extends-classes');

class CommonToolbarEvents 
{
    onSave = () => {
        console.log('save')
    }
    onSaveAll = () => {
        console.log('save all')        
    }
}

class CppDebugToolbarEvents 
{
    onDebug = () => {
        console.log('on debug')
    }
    onNextLine = () => {
        console.log('next line')
    }
    onStepInto = () => {
        console.log('step in')
    }
    onStepOut = () => {
        console.log('step out')
    }
}

class CMakeToolbarEvents extends Classes(CommonToolbarEvents, CppDebugToolbarEvents)
{
    onBuild = () => {
        console.log('build')
    }
    onBuildRun = () => {
        console.log('build_run')
    }
    onCancel = () => {
        console.log('cancel')
    }
    onRun = () => {
        console.log('run')
    }
    onCmake = () => {
        console.log('cmake')
    }
}

/**
 * Main Window
 */
class MainWindow extends React.Component 
{
    state = 
    {
        monacoOptions: 
        {
            theme: 'vs-dark',
            options: {}
        }
    }

    handleTreeUpdates(head, data)
    {
        if (head.tree.flat === true) {
            this.props.dispatch(setFileTreeBranch(head.origin, head.tree.files, head.tree.directories));
        }
    }

    onDataStream(head, data)
    {
        if (head.type === undefined || head.type === null) {
            console.error("backend didn't send a message type. notify this to the backend dev");
            return;
        }

        if (head.type === "file_tree") {
            this.handleTreeUpdates(head, data);
            return;
        }

        if (head.type === "fileContent") {
            console.log(head);
            let data = '';
            if (head.chunks !== undefined)
                data = head.chunks.join();
            this.props.dispatch(addOpenFileWithContent(head.path, data));
            return;
        }
        console.log(head);
    }

    onControlStream(head, data)
    {
        console.log(head);
    }

    onStreamError(err)
    {
        console.error(err);
    }

    constructor(props) 
    {
        super(props)
        this.registerMenuActions();

        this.backend = new Backend
        (
            props.store, 
            // Control Callback
            (...args) => {this.onControlStream(...args);}, 
            // Data Callback
            (...args) => {this.onDataStream(...args);}, 
            // Error Callback
            (...args) => {this.onStreamError(...args);}
        );
    }

    registerMenuActions = () => 
    {
        ipcRenderer.on('openWorkspace', (event, arg) => {
            if (arg.canceled)
                return;
            this.backend.workspace().openWorkspace(arg.filePaths[0]);
        })

        ipcRenderer.on('connectBackend', (event, arg) => {
            this.backend.attachToStreams();
        })
        
        ipcRenderer.on('testBackend', (event, arg) => {
            this.backend.workspace().openWorkspace("D:/Development/IDE2/test-project");
        })
    }

    render = () => 
    {
        return (
            <div id='Content'>
                <Toolbar cmake={new CMakeToolbarEvents()}/>
                <div id='SplitterContainer'>
                    <SplitterLayout vertical={false} percentage={true} secondaryInitialSize={60}>
                        <div>
                            <Explorer backend={this.backend}/>
                        </div>
                        <div id='RightOfExplorer'>
                            <SplitterLayout vertical={true} secondaryInitialSize={250}>
                                <Editor className='Editor' monacoOptions={this.state.monacoOptions}></Editor>
                                <LogsAndOthers className="logsAndOthers"></LogsAndOthers>
                            </SplitterLayout>
                        </div>
                    </SplitterLayout>
                </div>
            </div>
        )
    }
}

export default connect()(MainWindow);