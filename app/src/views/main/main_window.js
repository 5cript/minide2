import React, { useCallback } from 'react';

// Components
import Toolbar from './components/toolbar';
//import FileView from './components/file_tree';
import Explorer from './components/explorer';
import SplitterLayout from 'react-splitter-layout';
import Editor from './components/editor';
import LogsAndOthers from './components/logs_and_term';
import {connect} from 'react-redux';
import MessageBox from '../../elements/message_box';

// Actions
import {setFileTreeBranch} from '../../actions/workspace_actions';
import {addOpenFileWithContent, activeFileWasSynchronized, fileWasSynchronized} from '../../actions/open_file_actions';

// Other
import Backend from '../../backend_connector';
import _ from 'lodash';
import Dictionary from '../../util/localization';

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
        },
        yesNoBoxVisible: false,
        yesNoMessage: 'blubber'
    }

    isShortcut(event, shortcutDefinition)
    {
        if (shortcutDefinition === undefined)
            console.error('oups, shortcut is not defined?! look at shortcut store');

        if (event.type !== 'keyup')
            return;

        let is = true;
        _.forIn(shortcutDefinition, (v, k) => 
        {
            if (k === 'key')
            {
                if (event[k].toLowerCase() !== v)
                {
                    is = false;
                    return false;
                }
            }
            else if (event[k] !== v)
            {
                is = false;
                return false;
            }
        });
        return is;
    }

    /**
     * Handles all pressed window-wide shortcuts
     */
    installSaveShortcuts()
    {
        window.addEventListener('keyup', (event) => 
        {
            if (this.isShortcut(event, this.props.shortcuts.bindings.save))
            {
                if (this.props.activeFile >= 0) 
                {
                    let file = this.props.openFiles[this.props.activeFile];
                    this.backend.workspace().saveFile(file.path, file.content, () => 
                    {
                        this.props.dispatch(activeFileWasSynchronized());
                    });
                }
                else
                    console.log('no open file');
                return;
            }

            if (this.isShortcut(event, this.props.shortcuts.bindings.saveAll))
            {
                this.props.openFiles.map(file => 
                {
                    console.log(file);
                    if (!file.synchronized)
                    {
                        this.backend.workspace().saveFile(file.path, file.content, () => 
                        {
                            this.props.dispatch(fileWasSynchronized(file.path));
                        });
                    }
                })
            }
        }, true);
    }

    handleTreeUpdates(head, data)
    {
        if (head.tree.flat === true) 
        {
            console.log(head.origin);
            this.props.dispatch(setFileTreeBranch(head.origin, head.tree.files, head.tree.directories));
        }
    }

    onDataStream(head, data)
    {
        if (head.type === undefined || head.type === null) 
        {
            console.error("backend didn't send a message type. notify this to the backend dev");
            return;
        }

        if (head.type === "file_tree") 
        {
            this.handleTreeUpdates(head, data);
            return;
        }

        if (head.type === "fileContent") 
        {
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
        this.installSaveShortcuts();

        this.dict = new Dictionary();
        this.dict.setLang(this.props.locale.language);

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
        ipcRenderer.on('openWorkspace', (event, arg) => 
        {
            if (arg.canceled)
                return;
            this.backend.workspace().openWorkspace(arg.filePaths[0]);
        })

        ipcRenderer.on('connectBackend', (event, arg) => 
        {
            this.backend.attachToStreams();
        })
        
        ipcRenderer.on('testBackend', (event, arg) => 
        {
            this.backend.workspace().openWorkspace("D:/Development/IDE2/test-project");
        })
        
        ipcRenderer.on('closeIssued', (event, arg) => 
        {
            // any unchanged files?
            let anyFound = false;
            for (let i in this.props.openFiles)
            {
                const file = this.props.openFiles[i];
                if (!file.synchronized)
                {
                    this.showYesNoBox(this.dict.translate('$CloseWithUnsavedChanges', 'dialog'), () => {
                        ipcRenderer.sendSync('closeNow', '');
                    })
                    anyFound = true;
                    break;
                }
            }
            if (!anyFound)
                ipcRenderer.sendSync('closeNow', '');
        })
    }

    showYesNoBox(message, yesAction) 
    {
        this.setState({
            yesNoBoxVisible: true,
            yesNoMessage: message
        })
        this.yesAction = yesAction;
    }

    onMessageBoxClose(whatButton)
    {
        this.setState({
            yesNoBoxVisible: false
        });
        if (whatButton === "Yes")
            this.yesAction();
    }

    render = () => 
    {
        return (
            <div id='Content'>
                <Toolbar cmake={new CMakeToolbarEvents()}/>
                <div id='SplitterContainer'>
                    <SplitterLayout vertical={false} percentage={true} secondaryInitialSize={60}>
                        <div>
                            <Explorer dict={this.dict} backend={this.backend}/>
                        </div>
                        <div id='RightOfExplorer'>
                            <SplitterLayout vertical={true} secondaryInitialSize={250}>
                                <Editor dict={this.dict} className='Editor' monacoOptions={this.state.monacoOptions}></Editor>
                                <LogsAndOthers dict={this.dict} className="logsAndOthers"></LogsAndOthers>
                            </SplitterLayout>
                        </div>
                    </SplitterLayout>
                    <MessageBox boxStyle="YesNo" dict={this.dict} visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onMessageBoxClose(wb);}}/>
                </div>
            </div>
        )
    }
}

export default connect(state => {
    return {
        openFiles: state.openFiles.openFiles,
        activeFile: state.openFiles.activeFile,
        shortcuts: state.shortcuts,
        locale: state.locale
    }
})(MainWindow);