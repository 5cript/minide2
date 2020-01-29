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
import {openWorkspace} from '../../actions/workspace_actions';

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
class MainWindow extends React.Component {
    state = {
        monacoOptions: {
            theme: 'dark',
            options: {}
        }
    }

    constructor(props) {
        super(props)
        this.registerMenuActions();
    }

    registerMenuActions = () => {
        ipcRenderer.on('openWorkspace', (event, arg) => {
            if (arg.canceled)
                return;
            this.props.dispatch(openWorkspace(arg.filePaths[0]))
        })
    }

    render = () => {
        return (
            <div id='Content'>
                <Toolbar cmake={new CMakeToolbarEvents()}/>
                <div id='SplitterContainer'>
                    <SplitterLayout vertical={false} percentage={true} secondaryInitialSize={60}>
                        <div>
                            <Explorer/>
                        </div>
                        <div id='RightOfExplorer'>
                            <SplitterLayout vertical={true} secondaryInitialSize={250}>
                                <Editor monacoOptions={this.state.monacoOptions}></Editor>
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