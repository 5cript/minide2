import React from 'react';

// Components
import Editor, { monaco } from '@monaco-editor/react';
import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';
import IconButton from '@material-ui/core/IconButton';
import { MuiThemeProvider, createMuiTheme } from '@material-ui/core/styles';
import { styled } from '@material-ui/core/styles';

// Other
import {pathModifier} from '../../../util/path_util';

// Redux
import {connect} from 'react-redux';

// Style
import './styles/editor.css';
import './styles/tabs.css';
import { setActiveFile, removeOpenFile } from '../../../actions/open_file_actions';

// Requires
const path = require('path');

const SimpleIconButton = styled(IconButton)({
    borderRadius: '0px',
    padding: '5px',
    marginLeft: '3px'
});

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

//__dirname = "D:/Development/IDE2/app";

function uriFromPath(_path) {
    let pathName = path.resolve(_path).replace(/\\/g, '/');

    if (pathName.length > 0 && pathName.charAt(0) !== '/') {
        pathName = `/${pathName}`;
    }
    return encodeURI(`file://${pathName}`);
}

monaco.config({
    urls: {
        monacoLoader: uriFromPath(
            path.join(__dirname, '../public/vs/loader.js')
        ),
        monacoBase: uriFromPath(
            path.join(__dirname, '../public/vs')
        )
    }
});

function MonacoEditorComponent(props) {
    const [isEditorReady, setIsEditorReady] = React.useState(false);
    const valueGetter = React.useRef(null);

    function handleEditorDidMount(_valueGetter) {
        setIsEditorReady(true);
        valueGetter.current = _valueGetter;
    }

    return (
        <Editor
            height="100%"
            width="100%"
            language="cpp"
            theme={props.monacoOptions.theme}
            value="// write your code here"
            editorDidMount={handleEditorDidMount}
            options={props.monacoOptions.options}
        />
    );
}

class CodeEditor extends React.Component {
    onTabChange = (index, lastIndex, event) => {
        this.props.dispatch(setActiveFile(index));
        return true;
    }

    constructor(props) {
        super(props)
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
                                        <Tab key={file}>{pathModifier.shorten(file)}
                                            <button id='x' onClick={(e) => {this.props.dispatch(removeOpenFile(file)); e.stopPropagation()}}></button>
                                        </Tab>
                                    );
                                })
                            }
                        </TabList>
                        {
                            this.props.openFiles.map((file) => {
                                return <TabPanel key={file}></TabPanel>
                            })
                        }
                    </Tabs>
                    <MonacoEditorComponent monacoOptions={this.props.monacoOptions} id='MonacoWrap' language="javascript" />
                </MuiThemeProvider>
            </div>
        )
    }
}

export default connect(
    state => state.openFiles
)(CodeEditor);