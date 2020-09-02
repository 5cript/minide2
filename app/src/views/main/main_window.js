import React from 'react';
import {connect} from 'react-redux';

// Components
import Toolbar from './components/toolbar';
//import FileView from './components/file_tree';
import Explorer from './components/explorer';
import SplitterLayout from 'react-splitter-layout';
import Editor from './components/editor';
import LogsAndOthers from './components/logs_and_term';
import MessageBox from '../../elements/message_box';
import Blocker from './components/toolbar_blocker';
import Slide from 'react-reveal/Slide';
import {DragDropContext} from 'react-beautiful-dnd';
import KeybindActor from './keybind_actor';
import CommonActions from './common_actions';

// Actions
import {setFileTreeBranch, setActiveProject} from '../../actions/workspace_actions';
import {addOpenFileWithContent, setActiveFile, moveOpenFile} from '../../actions/open_file_actions';
import {setConnected, setConnectMessage, setTryingToConnect, setSessionId, setBackendPort, setBackendIp} from '../../actions/backend_actions';
import {initializeToolbars} from '../../actions/toolbar_actions';
import {addToLog, clearLog, focusLogByName, setLogType, moveLogs} from '../../actions/log_actions.js';
import {setPreferences} from '../../actions/preferences_actions.js';

// Other
import Backend from '../../backend_connector';
import _ from 'lodash';
import Dictionary from '../../util/localization';
import LocalPersistence from '../../util/persistence';

// Style
import './styles/main.css'
import ReactResizeDetector from 'react-resize-detector';
import InputBox from '../../elements/input_box';

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
        yesNoBoxVisible: false,
        yesNoMessage: '',
        okBoxVisible: false,
        okBoxMessage: '',
        logsHeight: 200,
        inputForm: null,
        inputFormEnvs: []
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
     * Register and Forwards window wide shortcuts
     */
    installShortcuts()
    {
        window.addEventListener('keyup', event => 
        {
            if (this.keybindActor)
                this.keybindActor.onKey(event);
        }, true);
    }

    handleTreeUpdates(head, data)
    {
        let firstLoad = false
        if (_.isEmpty(this.props.workspaceRoot))
        {
            firstLoad = true;
        }

        if (head.tree.flat === true) 
        {
            this.props.dispatch(setFileTreeBranch(head.origin, head.tree.files, head.tree.directories));
        }

        if (firstLoad && this.props.preferences.backend.autoLoadLastProject)
        {
            const lastActive = this.persistence.getLastActive(this.currentHost());
            console.log(lastActive)
            if (lastActive)
            {
                this.backend.workspace().setActiveProject(lastActive, () => {
                    this.props.dispatch(setActiveProject(lastActive))
                    this.onActiveProjectChange(lastActive);
                });
            }
        }
    }

    initToolbars = (json) =>
    {
        this.props.dispatch(initializeToolbars(json));
        if (this.toolbar)
            this.toolbar.preselectToolbar();
    }

    showProjectSettigns({settingsFile})
    {
        if (this.props.activeProject === undefined || this.props.activeProject === null || this.props.activeProject === '')
            return this.showOkBox(this.dict.translate("$NoActiveProject", "dialog"));
        if (this.props.workspaceRoot === undefined || this.props.workspaceRoot === null || this.props.workspaceRoot === '')
            return;

        this.backend.workspace().loadFile(this.props.activeProject + "/.minIDE/" + settingsFile, "projectSettings");
    }

    handleLuaRpc(func, data)
    {
        switch (func)
        {
            case('showProjectSettings'):
                return this.showProjectSettigns(data);
            case('setComboboxData'):
                return this.toolbar.comboboxLoaded(
                    data.toolbarId,
                    data.itemId,
                    data.items
                )
            case('actionCompleted'):
            {
                if (this.toolbar)
                {
                    this.toolbar.setItemNotRunning(data.toolbarId, data.itemId);
                }

                break;
            }
            default:
                return;
        }
    }

    onControlStream(head, data)
    {
        try
        {
            if (head.type === "welcome")
                ipcRenderer.sendSync('haveCookieUpdate', {});
            else if (head.type === "keep_alive")
            {}
            else if (head.type === "lua_rpc")
            {
                this.handleLuaRpc(head.functionName, JSON.parse(head.data))
            }
            else if (head.type === "lua_process")
            {
                if (head.message === "\x1b[2J")
                {
                    this.props.dispatch(setLogType(head.processName, head.kind));
                    this.props.dispatch(clearLog(head.processName));
                    this.props.dispatch(focusLogByName(head.processName));
                }
                else
                    this.props.dispatch(addToLog(head.processName, head.message));
            }
            else if (head.type === "lua_process_info")
            {
                const info = JSON.parse(head.data);
                if (info.what === "processEnded")
                {
                    let message = head.processName + " " + this.dict.translate("$ProcessEnded", "lua") + " " + info.status + "\n";
                    this.props.dispatch(addToLog(head.processName, message));
                }
                else if (info.what === "processStartFailure")
                {
                    let message = head.processName + " " + this.dict.translate("$ProcessStartFail", "lua") + " " + info.error + "\n";
                    this.props.dispatch(addToLog(head.processName, message));
                    if (info.error === 2)
                        this.props.dispatch(addToLog(head.processName, this.dict.translate("$ProcessNotFound", "lua") + "\n"));
                    if (info.command !== undefined)
                        this.props.dispatch(addToLog(head.processName, info.command));
                }
            }
            else if (head.type === "create_input_form")
            {
                console.log(head.specs);
                const spec = JSON.parse(head.specs);
                console.log(spec);

                let schema = {};

                schema.categories = spec.categories.map(category => {
                    return {
                        id: category.id,
                        caption: this.dict.translate(category.label, spec.dictionary),
                        borderColor: category.border_color
                    }
                })
                schema.fields = spec.fields.map(field => {
                    let type = field.type;
                    if (type === "text")
                        type = "input";
                    return {
                        key: field.id,
                        label: this.dict.translate(field.label, spec.dictionary),
                        type: type,
                        category: field.category
                    }
                })
                this.setState({inputForm: schema, inputFormEnvs: spec.environments ? spec.environments : []});
            }
            else if (head.type === "toggle_source_header")
            {
                const checkProjectSettings = (onSplitWasDecided, onLocalWasDecided) => 
                {
                    this.backend.workspace().loadProjectMetafile
                    (
                        options => 
                        {
                            if ((head.specialDirExists || head.specialDirFile !== "") && options.splitSourceAndInclude !== true && options.ignoreSeemingSplit !== true) 
                            {
                                this.showYesNoBox(this.dict.translate('$LooksLikeSourceSplitButNotConfigured', 'project'), 
                                    () => // yes, want to split
                                    {
                                        this.backend.workspace().injectProjectSettings({
                                            splitSourceAndInclude: true,
                                            ignoreSeemingSplit: false
                                        });
                                        onSplitWasDecided();
                                    },
                                    () => // no, dont want split
                                    {
                                        this.backend.workspace().injectProjectSettings({
                                            splitSourceAndInclude: false,
                                            ignoreSeemingSplit: true
                                        });
                                        onLocalWasDecided();
                                    }
                                );
                            }
                            else if (options.splitSourceAndInclude !== true && options.ignoreSeemingSplit === true)
                            {
                                onLocalWasDecided();
                            }
                            else if (options.splitSourceAndInclude === true && head.specialDirFile !== "")
                            {
                                onSplitWasDecided();
                            }
                            else if (head.specialDirFile === "" || !head.fileInSpecialDir)
                            {
                                onLocalWasDecided();
                            }
                            else
                            {
                                this.showOkBox('implementation_error, uncaught variation in file split decision:\n' + JSON.stringify(options) + "\n" + JSON.stringify(head));
                                console.error('implementation_error, uncaught variation in file split decision', options, head);
                            }
                        }, 
                        error => 
                        {
                            console.error(JSON.stringify(error));
                        }
                    )
                };

                const openFile = (correspondingFile) => 
                {
                    const fileIfOpen = this.props.openFiles.findIndex(file => file.path === correspondingFile)
                    if (fileIfOpen !== -1)
                        this.props.dispatch(setActiveFile(fileIfOpen));
                    else
                        this.backend.workspace().loadFile(correspondingFile, undefined, fail => 
                        {
                            if (fail.fileNotFound === true) 
                            {
                                this.showYesNoBox(this.dict.translate('$FileNotFoundShallCreate', 'project') + "\n" + fail.path, () => 
                                {
                                    this.backend.workspace().createFile(correspondingFile, createFail => 
                                    {
                                        this.showOkBox(this.dict.translate('$CouldNotCreateFile', 'project') + "\n" + createFail)
                                    })
                                });
                            }
                            else 
                                this.showOkBox('Error: ' + fail);
                        });
                }
                checkProjectSettings
                (
                    () => {/* split decided */
                        openFile(head.specialDirFile);
                    },
                    () => {/* local decided */
                        openFile(head.inplaceFile);
                    }
                );
            }
            else
            {
                // Unhandled:
                console.log(head);
            }
        }
        catch(e)
        {
            console.error(e);
        }
    }

    callOnEditor = (fn) => 
    {
        if (this.editor)
        {
            console.log(this.editor)
            const monaco = this.editor.getMonaco();
            if (monaco)
            {
                fn(monaco);
            }
        }
    }

    onDataStream(head, data)
    {
        try
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

            if (head.type === "file_content") 
            {
                let data = '';
                if (head.chunks !== undefined)
                    data = head.chunks.join();
                this.callOnEditor(monaco => {
                    monaco.updateFileModel({
                        uri: head.path, 
                        isAbsolute: head.isAbsolutePath, 
                        data: data,
                        focusLineNumber: head.line,
                        focusColumn: head.linePos
                    });
                })
                this.props.dispatch(addOpenFileWithContent(head.path, head.isAbsolutePath, data));
                return;
            }

            if (head.type === "welcome")
            {
                this.props.dispatch(setConnected(true));
                this.backend.toolbar().loadAll(res => {
                    res.json().then(json => {
                        this.initToolbars(json);
                    })
                });
                if (this.props.preferences.backend.autoLoadWorkspace === true)
                {
                    const wspace = this.persistence.getLastWorspace(this.currentHost())
                    if (wspace !== undefined && wspace !== null && wspace !== '')
                    {
                        this.backend.workspace().openWorkspace(wspace);
                    }
                }
            }
        }
        catch(e)
        {
            console.error(e);
        }
    }

    onStreamError(err)
    {
        console.error(err);
    }

    constructor(props) 
    {
        super(props)
        this.dict = new Dictionary();
        this.dict.setLang(this.props.locale.language);

        this.registerIpcHandler();
        this.installShortcuts();

        this.props.dispatch(setConnectMessage(this.dict.translate("$ConnectingToBackend", "main_window")));

        this.backend = new Backend
        (
            props.store, 
            // Control Callback
            (...args) => {this.onControlStream(...args);}, 
            // Data Callback
            (...args) => {this.onDataStream(...args);}, 
            // Error Callback
            (...args) => {this.onStreamError(...args);},
            // on Connection Loss
            (...args) => {this.onConnectionLoss(...args);}
        );
        
        this.commonActions = new CommonActions
        (
            props.store,
            this,
            this.backend
        );
    }

    onConnectionLoss(which)
    {
        if (which === "control_error" || which === "data_error")
            this.props.dispatch(setConnectMessage(this.dict.translate("$ConnectionFailed", "main_window")))
        else
            this.props.dispatch(setConnectMessage(this.dict.translate("$ConnectionLost", "main_window")))
        this.props.dispatch(setConnected(false));
        this.props.dispatch(setTryingToConnect(false));
    }

    currentHost = () => 
    {
        return {
            host: this.props.backend.ip,
            port: this.props.backend.port
        };
    }

    loadKeybindsIfPossible = () => 
    {
        if (this.home && this.keybindActor)
            this.keybindActor.loadKeybindsFromDrive(this.home);
    }

    registerIpcHandler = () => 
    {
        this.debouncedStart = _.debounce(() => {
            this.backend.authenticate(() => {this.backend.readControl()});
        }, 300)

        ipcRenderer.on('openWorkspace', (event, arg) => 
        {
            if (arg.canceled)
                return;
            this.backend.workspace().openWorkspace(arg.filePaths[0]);
            this.persistence.setLastWorkspace(this.currentHost(), arg.filePaths[0]);
        })

        ipcRenderer.on('setHome', (event, arg) => {
            this.home = arg;
            this.loadKeybindsIfPossible();

            this.persistence = new LocalPersistence(this.home, window.require('fs'));
            try
            {
                this.persistence.load();
            }
            catch(e)
            {
                try
                {
                    this.persistence.save();
                }
                catch(e)
                {}
            }
        })

        ipcRenderer.on('preferences', (event, arg) => {
            this.props.dispatch(setPreferences(arg));
        })

        ipcRenderer.on('setBackend', (event, arg) =>
        {
            this.props.dispatch(setBackendIp(arg.ip));
            this.props.dispatch(setBackendPort(arg.port));
            if (arg.autoConnect && this.props.backend.connected === false)
                this.debouncedStart()
        })

        ipcRenderer.on('connectBackend', (event, arg) => 
        {
            this.props.dispatch(setTryingToConnect(true));
            this.backend.authenticate(() => {this.backend.readControl()});
        })
        
        ipcRenderer.on('testBackend', (event, arg) => 
        {
            this.showYesNoBox('Bla Bla\nBlubber Asdf\n1234');
            //this.backend.workspace().openWorkspace("D:/Development/IDE2/test-project");
        })

        ipcRenderer.on('reloadToolbar', (event, arg) => 
        {
            this.backend.toolbar().loadAll(res => {
                res.json().then(json => {
                    this.initToolbars(json);
                })
            });
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

        ipcRenderer.on('cookie', (event, arg) => {
            if (arg.name === 'aSID')
            {
                this.props.dispatch(setSessionId(arg.value));
                this.backend.readData();
            }
        })
    }

    showYesNoBox(message, yesAction, noAction) 
    {
        this.setState({
            yesNoBoxVisible: true,
            yesNoMessage: message
        })
        this.yesAction = yesAction;
        this.noAction = noAction;
    }

    showOkBox(message, okAction) 
    {
        this.setState({
            okBoxVisible: true,
            okBoxMessage: message
        })
        this.okAction = okAction;
    }

    onMessageBoxClose(whatButton)
    {
        this.setState({
            yesNoBoxVisible: false
        });
        if (whatButton === "Yes" && this.yesAction)
            this.yesAction();
        else if (whatButton === "No" && this.noAction)
            this.noAction();
    }

    onOkBoxClose(whatButton)
    {
        this.setState({
            okBoxVisible: false
        });
        if (whatButton === "Ok" && this.okAction)
            this.okAction();
    }

    componentDidMount()
    {
    }

    setToolbarRef = (node) => 
    {
        this.toolbar = node;
    }

    onActiveProjectChange = (proj) => 
    {
        try
        {
            if (this.toolbar)
                this.toolbar.onActiveProjectChange(proj)
        }
        catch(err)
        {
            console.log(err);
        }
    }

    setLogsRef = (node) => 
    {
        this.logsAndTerminal = node;
    }

    dndDragEnd = (dndOperation) =>
    {
        if (dndOperation.source === null || dndOperation.source === undefined || dndOperation.destination === null || dndOperation.destination === undefined)
            return;

        // drops in the same drop zone. for example for logtabs
        if (dndOperation.source.droppableId === dndOperation.destination.droppableId)
        {
            if (dndOperation.source.droppableId === "dropzone_mainLogTabs")
            {
                this.props.dispatch(moveLogs(dndOperation.source.index, dndOperation.destination.index));
            }
            else if (dndOperation.source.droppableId === "dropzone_editorTabs")
            {
                this.props.dispatch(moveOpenFile(dndOperation.source.index, dndOperation.destination.index));
            }
        }
    }

    setEditorRef = (editor) => 
    {
        this.editor = editor;
    }

    setKeybindActor = (actor) => 
    {
        this.keybindActor = actor;
        this.loadKeybindsIfPossible();
    }

    onDeleteFile = (file) =>
    {
        this.showYesNoBox(this.dict.translate('$ReallyDeleteFile', 'file_tree') + "\n" + file, () => {
            this.backend.workspace().deleteFile(file);
        })
    }

    render = () => 
    {
        return (
            <div id='Content'>
                <DragDropContext
                    onDragEnd={(...args) => {
                        this.dndDragEnd(...args)
                    }}
                >
                <div id='BlockerOrToolbar'>
                    <Slide left when={!this.props.backend.connected}>
                        <Blocker></Blocker>
                    </Slide>
                    <Slide right when={this.props.backend.connected}>
                        <Toolbar 
                            dict={this.dict} 
                            ref={(n) => {this.setToolbarRef(n)}} 
                            backend={this.backend} 
                            cmake={new CMakeToolbarEvents()}
                            commonActions={this.commonActions}
                        />
                    </Slide>
                </div>
                <div id='SplitterContainer'>
                    <SplitterLayout vertical={false} percentage={true} secondaryInitialSize={75}>
                        <div>
                            <Explorer 
                                onActiveProjectSet={this.onActiveProjectChange} 
                                onDeleteFile={this.onDeleteFile}
                                persistence={this.persistence} 
                                dict={this.dict} 
                                backend={this.backend}
                            />
                        </div>
                        <div id='RightOfExplorer'>
                            <SplitterLayout 
                                vertical={true} 
                                secondaryInitialSize={250}
                            >
                                <Editor ref={this.setEditorRef} dict={this.dict} className='Editor'></Editor>
                                <ReactResizeDetector handleWidth handleHeight>
                                    {({width, height}) =>
                                        <LogsAndOthers 
                                            dict={this.dict} 
                                            ref={this.setLogsRef}
                                            backend={this.backend}
                                            width={width}
                                            height={height} 
                                            className="logsAndOthers"
                                            tabsId="mainLogTabs"
                                        ></LogsAndOthers>
                                    }
                                </ReactResizeDetector>
                            </SplitterLayout>
                        </div>
                    </SplitterLayout>
                </div>
                <MessageBox boxStyle="YesNo" dict={this.dict} visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onMessageBoxClose(wb);}}/>
                <MessageBox boxStyle="Ok" dict={this.dict} visible={this.state.okBoxVisible} message={this.state.okBoxMessage} onButtonPress={(wb)=>{this.onOkBoxClose(wb);}}/>
                <MessageBox
                    boxStyle="Modal"
                    dict={this.dict} 
                    visible={this.state.inputForm !== null && this.state.inputForm !== undefined} 
                    message={''}
                    disableInput={true}
                    height={500}
                    width={500}
                    disableResize={false}
                >
                    <div className="dynamicInputFormContainment">
                        <InputBox
                            environments={this.state.inputFormEnvs}
                            visible={this.state.inputForm !== null && this.state.inputForm !== undefined}
                            onButtonPress={(b, data) => {
                                switch(b)
                                {
                                    case("Ok"):
                                    {
                                        console.log(data);
                                        this.setState({inputForm: null});
                                    }
                                    case("Cancel"):
                                    {
                                        this.setState({inputForm: null});
                                    }
                                }
                            }}
                            dict={this.dict}
                            schema={this.state.inputForm ? this.state.inputForm : {
                                fields: [],
                                categories: []
                            }}
                        >
                        </InputBox>
                    </div>
                </MessageBox>
                <KeybindActor 
                    ref={this.setKeybindActor}
                    backend={this.backend}
                    mainWindow={this}
                    dict={this.dict}
                    commonActions={this.commonActions}
                ></KeybindActor>
                </DragDropContext>
            </div>
        )
    }
}

export default connect(state => {
    return {
        openFiles: state.openFiles.openFiles,
        activeFile: state.openFiles.activeFile,
        locale: state.locale,
        backend: state.backend,
        activeProject: state.workspace.activeProject,
        workspaceRoot: state.workspace.root,
        preferences: state.preferences.preferences
    }
})(MainWindow);