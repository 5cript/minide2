import React from 'react';

// Components
import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';
import { MuiThemeProvider, createMuiTheme } from '@material-ui/core/styles';
import MonacoEditor from 'react-monaco-editor';
import ReactResizeDetector from 'react-resize-detector';
import MessageBox from '../../../elements/message_box';

// Other
import {pathModifier} from '../../../util/path_util';
import extensionToLanguage from '../../../util/extension_to_lang';

// Redux
import {connect} from 'react-redux';
import {setActiveFile, removeOpenFile, setActiveFileContent} from '../../../actions/open_file_actions';

// Style
import './styles/editor.css';

/*
const SimpleIconButton = styled(IconButton)({
    borderRadius: '0px',
    padding: '5px',
    marginLeft: '3px'
});
*/

// README:
// Multiple models/tabs/open_files: https://github.com/react-monaco-editor/react-monaco-editor/issues/67

// VERY NICE HELP:
// https://microsoft.github.io/monaco-editor/playground.html#interacting-with-the-editor-rendering-glyphs-in-the-margin

const HoverFix = createMuiTheme({
    overrides: {
        MuiIconButton: {
            root: {
                '&:hover': {
                    backgroundColor: 'var(--background-color-brighter)'
                }
            }
        }
    }
})

class MonacoEditorComponent extends React.Component
{
    state = 
    {
        theme: 'vs-dark',
        options: {
            glyphMargin: false
        },        
    }

    constructor(props)
    {
        super(props)

        this.currentPath = null;
        this.viewStates = {};
        this.positions = {};
        this.voidModel = null;
        this.voidModelPath = '/_/mindide/voidmodel';
    }

    onMount = (editor, monaco) => 
    {
        this.editor = editor;
        this.monaco = monaco;
        editor.focus();
        this.voidModel = this.monaco.editor.createModel('', undefined, this.monaco.Uri.parse(this.voidModelPath));
        this.setModel(this.voidModelPath);
    }

    openFileContent = () => 
    {
        if (this.props.activeFile > -1)
            return this.props.openFiles[this.props.activeFile].content;
        return "";
    }

    updateFileModel = ({uri, data, focusLineNumber, focusColumn}) => 
    {
        if (this.monaco === undefined)
            return;

        const muri = this.monaco.Uri.parse(uri);
        let model = this.monaco.editor.getModel(muri);
        if (model === null)
            model = this.monaco.editor.createModel(data, this.languageFromExtension(uri), muri);
        else
        {
            this.resetViewState(uri);
            model.setValue(data);
        }
        this.setModel(uri, model);

        if (focusLineNumber !== undefined && focusLineNumber !== null)
            this.jumpTo(focusLineNumber, focusColumn ? focusColumn : 0);
    }
    
    setModel = (path, model) => 
    {
        if (model === null || model === undefined)
            model = this.monaco.editor.getModel(this.monaco.Uri.parse(path));
        if (model === null || model === undefined)
            return;

        // save current editor states
        if (this.currentPath !== null)
        {
            this.positions[this.currentPath] = this.editor.getPosition();
            this.viewStates[this.currentPath] = this.editor.saveViewState();
        }
        this.currentPath = path;
        this.editor.setModel(model);

        // restore states
        if (this.viewStates[path])
        {
            this.editor.setPosition(this.positions[path]);
            this.editor.restoreViewState(this.viewStates[path]);
            this.editor.focus();
        }
    }

    resetViewState = (path) => 
    {
        delete this.viewStates[path];
        delete this.positions[path];
    }

    loadSelectedModel = () =>
    {
        if (this.props.activeFile === -1)
        {
            this.setModel(this.voidModelPath);
            return;
        }
        const path = this.props.openFiles[this.props.activeFile].path;
        const muri = this.monaco.Uri.parse(path);
        let model = this.monaco.editor.getModel(muri);
        this.setModel(path, model);
    }

    languageFromExtension = (path) => 
    {
        const filename = path.substring(path.lastIndexOf("/") + 1, path.length);

        const dotPosFirst = filename.indexOf('.');
        const dotPosLast = filename.lastIndexOf('.');
        if (dotPosFirst === dotPosLast)
            return extensionToLanguage(filename.substring(dotPosLast + 1, filename.length), filename, [], true);
        else
        {
            let res = extensionToLanguage(filename.substring(dotPosLast + 1, filename.length), filename, [], false);
            if (res === false)
                return extensionToLanguage(filename.substring(dotPosFirst + 1, filename.length), filename, [], true);
            return res;
        }
    }

    onChange = (value, event) =>
    {
        if (this.props.activeFile > -1)
            this.props.dispatch(setActiveFileContent(value));
    }

    compileMonacoOptions = () =>
    {
        if (this.monaco === undefined || this.editor === undefined)
            return this.state.options;

        let optionAddtions = {
            model: null
        }

        return {
            ...this.state.options,
            ...optionAddtions
        }
    }

    jumpTo = (line, column) =>
    {
        if (this.editor)
        {
            this.editor.revealPositionInCenter({lineNumber: line, column: column});
            this.editor.setPosition({lineNumber: line, column: column});
            this.editor.focus();
        }
    }

    componentDidUpdate = () =>
    {
        this.loadSelectedModel();
    }

    render() 
    {
        const options = this.compileMonacoOptions();

        return (
            <ReactResizeDetector
                handleHeight
                handleWidth
                onResize={()=> {
                    if (this.editor)
                        this.editor.layout();
                }}
            >
                <MonacoEditor
                    theme={this.state.theme}
                    onChange={(v, e) => {this.onChange(v, e)}}
                    editorDidMount={this.onMount}
                    options={options}
                />
            </ReactResizeDetector>
        );
    }
}

let ConnectedEditor = connect(state => {
    return {
        openFiles: state.openFiles.openFiles,
        activeFile: state.openFiles.activeFile
    }
}, null, null, {forwardRef: true})(MonacoEditorComponent);

class CodeEditor extends React.Component 
{
    state = {
        yesNoBoxVisible: false,
        yesNoMessage: 'blubber'
    }

    onTabChange(index)
    {
        this.props.dispatch(setActiveFile(index));
        return true;
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

    getMonaco = () =>
    {
        return this.monaco;
    }

    setMonacoRef = (monaco) => 
    {
        this.monaco = monaco;
    }

    render() 
    {
        return (
            <div id='EditorContainer'>
                <MuiThemeProvider theme={HoverFix}>
                    <Tabs onSelect={(e) => {this.onTabChange(e)}} selectedIndex={this.props.activeFile}>
                        <TabList>
                            {
                                this.props.openFiles.map((file, i) => {
                                    return (
                                        <Tab key={file.path}>{pathModifier.shorten(file.path)}
                                            {(() => {
                                                if (!file.synchronized)
                                                    return <svg viewBox="0 0 10 10" className="tabModifiedFile" xmlns="http://www.w3.org/2000/svg">
                                                        <circle cx="5" cy="5" r="5" fill="red"/>
                                                    </svg>
                                                else
                                                    return <div className="tabSpacer"></div>
                                            })()}
                                            <button id='closeCross' onClick={(e) => {
                                                if (!file.synchronized) 
                                                {
                                                    this.showYesNoBox(this.props.dict.translate("$CloseUnsavedWarning", "dialog"), () => {
                                                        this.props.dispatch(removeOpenFile(file.path));     
                                                    })
                                                }
                                                else
                                                    this.props.dispatch(removeOpenFile(file.path)); 
                                                e.stopPropagation();
                                            }}></button>
                                        </Tab>
                                    );
                                })
                            }
                        </TabList>
                        {
                            this.props.openFiles.map((file) => {
                                return <TabPanel key={file.path}></TabPanel>
                            })
                        }
                    </Tabs>
                </MuiThemeProvider>
                <ConnectedEditor ref={this.setMonacoRef} id='MonacoWrap' language="javascript" />
                <MessageBox boxStyle="YesNo" dict={this.props.dict} visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onMessageBoxClose(wb);}}/>
            </div>
        )
    }
}

export default connect(
    state => {
        return {
            openFiles: state.openFiles.openFiles,
            activeFile: state.openFiles.activeFile
        }
    }
, null, null, {forwardRef: true})(CodeEditor);