import React from 'react';

// Components
import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';
import { MuiThemeProvider, createMuiTheme } from '@material-ui/core/styles';
import MonacoEditor from 'react-monaco-editor';
import ReactResizeDetector from 'react-resize-detector';

// Other
import {pathModifier} from '../../../util/path_util';

// Redux
import {connect} from 'react-redux';
import { setActiveFile, removeOpenFile, setActiveFileContent } from '../../../actions/open_file_actions';

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

// Tabs:
// https://github.com/Microsoft/monaco-editor/issues/604

// TODO: Monaco security:
// https://github.com/suren-atoyan/monaco-react/issues/48#issuecomment-583863391

class MonacoEditorComponent extends React.Component
{
    onMount = (editor, monaco) => {
        this.editor = editor;
        editor.focus();
    }

    openFileContent = () => {
        if (this.props.activeFile > -1)
            return this.props.openFiles[this.props.activeFile].content;
        return "";
    }

    onChange = (value, event) => {
        if (this.props.activeFile > -1)
        {
            this.props.dispatch(setActiveFileContent(value));
        }
    }

    render() {
        console.log(this.openFileContent());

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
                    language="cpp"
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

class CodeEditor extends React.Component {
    onTabChange = (index, lastIndex, event) => {
        this.props.dispatch(setActiveFile(index));
        return true;
    }

    render = () => {
        return (
            <div id='EditorContainer'>
                <MuiThemeProvider theme={HoverFix}>
                    <Tabs onSelect={this.onTabChange} selectedIndex={this.props.activeFile}>
                        <TabList>
                            {
                                this.props.openFiles.map((file, i) => {
                                    return (
                                        <Tab key={file.path}>{pathModifier.shorten(file.path)}
                                            <button id='x' onClick={(e) => {this.props.dispatch(removeOpenFile(file.path)); e.stopPropagation()}}></button>
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
                <MonacoEditorComponent openFiles={this.props.openFiles} activeFile={this.props.activeFile} monacoOptions={this.props.monacoOptions} id='MonacoWrap' language="javascript" />
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