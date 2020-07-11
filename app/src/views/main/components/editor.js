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
import './styles/tabs.css';

/*
const SimpleIconButton = styled(IconButton)({
    borderRadius: '0px',
    padding: '5px',
    marginLeft: '3px'
});
*/

// README:
// Multiple models/tabs/open_files: https://github.com/react-monaco-editor/react-monaco-editor/issues/67

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
    onMount = (editor, monaco) => 
    {
        this.editor = editor;
        editor.focus();
    }

    openFileContent = () => 
    {
        if (this.props.activeFile > -1)
            return this.props.openFiles[this.props.activeFile].content;
        return "";
    }

    languageFromExtension = () => 
    {
        let path = '';
        if (this.props.activeFile > -1)
            path = this.props.openFiles[this.props.activeFile].path;
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
        console.log(this.editor.getSelection());
        if (this.props.activeFile > -1)
            this.props.dispatch(setActiveFileContent(value));
    }

    render() {
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
                    disabled={true}
                    language={this.languageFromExtension()}
                    theme={this.props.monacoOptions.theme}
                    onChange={(v, e) => {this.onChange(v, e)}}
                    value={this.openFileContent()}
                    editorDidMount={this.onMount}
                    options={this.props.monacoOptions.options}
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
})(MonacoEditorComponent);

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

    showYesNoBox(message) 
    {
        this.setState({
            yesNoBoxVisible: true,
            yesNoMessage: message
        })
    }

    onMessageBoxClose(whatButton)
    {
        this.setState({
            yesNoBoxVisible: false
        });
        if (whatButton === "Yes")
            this.yesAction();
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
                                            <button id='x' onClick={(e) => {
                                                if (!file.synchronized) 
                                                {
                                                    this.showYesNoBox(this.props.dict.translate("$CloseUnsavedWarning", "dialog"))
                                                    this.yesAction = () => {
                                                        console.log('yes action called');
                                                        this.props.dispatch(removeOpenFile(file.path));     
                                                    }
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
                <ConnectedEditor monacoOptions={this.props.monacoOptions} id='MonacoWrap' language="javascript" />
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
)(CodeEditor);