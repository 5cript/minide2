import React from 'react';

// Components
import MonacoEditor from 'react-monaco-editor';
import {withResizeDetector} from 'react-resize-detector';
import MessageBox from '../../../elements/message_box';
import {Droppable, Draggable} from 'react-beautiful-dnd';

// Other
import _ from 'lodash';
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

const getListStyle = isDraggingOver => ({
    display: 'flex',
    paddingBot: '0px',
    paddingLeft: '0px',
    overflow: 'auto',
    margin: '0px',
    width: '100%'
});
const getItemStyle = (isDragging, isSelected, draggableStyle) => ({
    // some basic styles to make the items look a bit nicer
    // change background colour if dragging
    paddingTop: '4px',
    backgroundColor: isDragging ? 'transparent' : (isSelected ? 'var(--background-color-darker)' : undefined),
    color: isSelected ? 'var(--theme-color)' : undefined,
    height: '25px',
  
    // styles we need to apply on draggables
    ...draggableStyle,
  });
  

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

        this.throttledModelLoad = _.debounce(() => {
            this.loadSelectedModel();
        }, 500);

        this.currentPath = null;
        this.viewStates = {};
        this.positions = {};
        this.voidModel = null;
        this.voidModelPath = '/_/mindide/voidmodel';
    }

    onMount = (editor, monaco) => 
    {
        this.props.refWorkaround(this);
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

    componentDidUpdate = (prevProps) =>
    {
        this.throttledModelLoad();

        if (this.props.width !== prevProps.width || this.props.height !== prevProps.height) {
            if (this.editor)
                this.editor.layout();
        }
    }

    render() 
    {
        const options = this.compileMonacoOptions();

        return (
            <MonacoEditor
                theme={this.state.theme}
                onChange={(v, e) => {this.onChange(v, e)}}
                editorDidMount={this.onMount}
                options={options}
            />
        );
    }
}

let ConnectedEditor = connect(state => {
    return {
        openFiles: state.openFiles.openFiles,
        activeFile: state.openFiles.activeFile
    }
}, null, null)(withResizeDetector(MonacoEditorComponent));

class CodeEditor extends React.Component 
{
    state = {
        yesNoBoxVisible: false,
        yesNoMessage: 'blubber'
    }

    onTabChange = (index) =>
    {
        this.props.dispatch(setActiveFile(index));
        return true;
    }

    showYesNoBox = (message, yesAction) =>
    {
        this.setState({
            yesNoBoxVisible: true,
            yesNoMessage: message
        })
        this.yesAction = yesAction;
    }

    onMessageBoxClose = (whatButton) =>
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
                <div className="editorTabBox">
                    <Droppable
                        droppableId={"dropzone_editorTabs"}
                        direction='horizontal'
                        className='editorTabZone'
                    >
                        {(provided, snapshot) => 
                            <div
                                ref={provided.innerRef}
                                style={getListStyle(snapshot.isDraggingOver)}
                                {...provided.droppableProps}
                            >
                                <div
                                    className='editorTabContainer'
                                >
                                    {this.props.openFiles.map((file, i) => {
                                        const label = pathModifier.shorten(file.path);
                                        return (
                                            <Draggable key={label + i} draggableId={'dropzone_editorTabs' + i} index={i}>
                                                {(prov, snap) => 
                                                    <div
                                                        ref={prov.innerRef}
                                                        className="editorTabHead"
                                                        {...prov.draggableProps}
                                                        {...prov.dragHandleProps}
                                                        style={getItemStyle(snap.isDragging, this.props.activeFile === i, prov.draggableProps.style)}
                                                        onClick={() => {this.onTabChange(i)}}
                                                    >
                                                        {label}
                                                        {(() => {
                                                            if (!file.synchronized)
                                                                return <svg viewBox="0 0 10 10" className="tabModifiedFile">
                                                                    <circle cx="5" cy="5" r="5" fill="red"/>
                                                                </svg>
                                                            else
                                                                return <div className="tabSpacer"></div>
                                                        })()}
                                                        <button id={'closeCross'} className='editorTabClose' onClick={(e) => {
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
                                                    </div>
                                                }
                                            </Draggable>
                                        )
                                    })}
                                </div>
                                {provided.placeholder}
                            </div>
                        }
                    </Droppable>
                </div>
                <ConnectedEditor refWorkaround={this.setMonacoRef} id='MonacoWrap' language="javascript" />
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