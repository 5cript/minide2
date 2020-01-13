import React from 'react';

// Components
import Toolbar from './components/toolbar';
//import FileView from './components/file_tree';
import Explorer from './components/explorer';
import SplitterLayout from 'react-splitter-layout';
import Editor from './components/editor';
import LogsAndOthers from './components/logs_and_term';

// Style
import './styles/main.css'

// requires
const Classes = require('extends-classes');

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

class MainWindow extends React.Component {
    state = {
        openFiles: ['C:/main.cpp', 'bla.hpp'],
        monacoOptions: {
            theme: 'dark',
            options: {}
        }
    }

    fileControl = (mw) => {return{
        closeFile: (file) => {
            let filtered = mw.state.openFiles.filter(f => f !== file);
            mw.setState({openFiles: filtered})
        },
        openFile: (file) => {
            mw.setState({openFiles: mw.state.openFiles.concat([file])});
        }
    }}

    render = () => {
        return (
            <div id='Content'>
                <Toolbar cmake={new CMakeToolbarEvents()}/>
                <div id='SplitterContainer'>
                    <SplitterLayout vertical={false} percentage={true} secondaryInitialSize={60}>
                        <div>
                            <Explorer openFiles={this.state.openFiles}/>
                        </div>
                        <div id='RightOfExplorer'>
                            <SplitterLayout vertical={true} percentage={true} secondaryInitialSize={20}>
                                <Editor monacoOptions={this.state.monacoOptions} openFiles={this.state.openFiles} fileControl={this.fileControl(this)}></Editor>
                                <LogsAndOthers></LogsAndOthers>
                            </SplitterLayout>
                        </div>
                    </SplitterLayout>
                </div>
            </div>
        )
    }
}

export default MainWindow;