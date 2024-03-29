import React, {PureComponent} from 'react';
import {connect} from 'react-redux';

// Components
import Tree from 'rc-tree';
import MessageBox from '../../../elements/message_box';
import ContextMenu from '../../../elements/context_menu';

// Utility
import classNames from 'classnames';
import _ from 'lodash';

// Actions
import {setActiveProject, setHoveredNode} from '../../../actions/workspace_actions';
import { setActiveFile } from '../../../actions/open_file_actions';

// Styles
import './styles/file_tree.css';

const Icon = ({ selected }) => (
    <div className={classNames('customized-icon')}> </div>
);
  
/*
Example

const treeData = [
    {
      key: '0-0',
      title: 'parent 1',
      children: [
        { key: '0-0-0', title: 'parent 1-1', children: [{ key: '0-0-0-0', title: 'parent 1-1-0' }] }
      ]
    },
  ];
*/

class FileView extends PureComponent 
{
    state = {
        yesNoBoxVisible: false,
        yesNoMessage: 'blubber'
    }

    constructor(props){
        super(props);

        this.treeRef = React.createRef();
    }

    setTreeRef = tree => 
    {
         this.tree = tree;
    };

    showYesNoBox(message, yesAction) 
    {
        this.setState({
            yesNoBoxVisible: true,
            yesNoMessage: message
        });
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

    onExpand = expandedKeys => 
    {
        let path = _.difference(expandedKeys, this.expandedKeys);
        this.expandedKeys = expandedKeys;

        if (path.length === 0)
            return;

        if (path[0] !== this.props.root)
            this.props.backend.workspace().enumDirectory(path[0]);
    };

    onTreeRightClick = event => 
    {
    }

    onNodeDoubleClick = (event) => 
    {
        if (this.hoveredNode === undefined)
            return;

        const node = _.cloneDeep(this.hoveredNode);

        // Directory
        if (this.hoveredNode.isLeaf === false)
            return this.props.backend.workspace().setActiveProject(node.key);

        // File
        let fileIndex = this.props.openFiles.findIndex(file => file.path === node.key);
        if (this.props.openFiles[fileIndex] === undefined)
            this.props.backend.workspace().loadFile(node.key).then((content) => {
                this.props.mainWindow.get().callOnEditor(monaco => {
                    monaco.updateFileModel({
                        uri: node.key, 
                        isAbsolute: false, // FIXME: <- figure out if it is
                        data: content,
                        focusLineNumber: 0,
                        focusColumn: 0
                    });
                })
            });
        else
            this.props.dispatch(setActiveFile(fileIndex));
    }

    onMouseEnter(v)
    {
        if (this.lockHoverChange || v.node.title === "")
            return;
        this.hoveredNode = v.node;
        this.props.dispatch(setHoveredNode(v.node.key));
    }

    onMouseLeave(v)
    {
        if (this.lockHoverChange || this.hoveredNode === undefined)
            return;
        this.hoveredNode = undefined;
        this.props.dispatch(setHoveredNode(undefined));
    }

    onFileContextOpen(xOffset, yOffset, event)
    {
        this.lockHoverChange = true;

        const doShow = this.hoveredNode && (this.hoveredNode.isLeaf !== false);
        if (doShow)
            this.contextItem = this.hoveredNode;
        return {
            x: event.clientX + xOffset,
            y: event.clientY +  yOffset,
            doShow: doShow
        }
    }

    onDirectoryContextOpen(xOffset, yOffset, event)
    {
        this.lockHoverChange = true;

        const doShow = this.hoveredNode && (this.hoveredNode.isLeaf === false);
        if (doShow)
            this.contextItem = this.hoveredNode;
        return {
            x: event.clientX + xOffset,
            y: event.clientY +  yOffset,
            doShow: doShow
        }
    }

    setActiveProject()
    {
        if (this.contextItem === undefined)
            console.error('context item undefined when setting active project')
        let contextItem = _.clone(this.contextItem);

        this.props.backend.workspace().setActiveProject(contextItem.key);
    }

    currentHost = () => 
    {
        return {
            host: this.props.backendState.ip,
            port: this.props.backendState.port
        };
    }
    
    render(){
        return (
            <div id="FileTreeContainer">
                <Tree
                    className={"main-tree"}
                    ref={this.setTreeRef}
                    treeData={[this.props.fileTree]}
                    onExpand={this.onExpand}
                    disabled={false}
                    showIcon={false}
                    selectable={false}
                    icon={Icon}
                    virtual={false}
                    onRightClick={e => this.onTreeRightClick(e)}
                    onDoubleClick={e => this.onNodeDoubleClick(e)}
                    onMouseEnter={pair => this.onMouseEnter(pair)}
                    onMouseLeave={pair => this.onMouseLeave(pair)}
                    switcherIcon={obj => {
                        if (obj.isLeaf) {
                            return <></>;
                        }
                        return (
                            <div style={{background: this.props.hoveredProps === obj.data.key ? "var(--background-color-brighter)" : "var(--background-color)"}}>
                                <i className={classNames("tree-arrow", obj.expanded ? "tree-down" : "tree-right")} style={{cursor: 'pointer'}}></i>
                            </div>
                        );
                    }}
                />
                <div style={{display: 'block', height: '100px'}}></div>
                <MessageBox boxStyle="YesNo" visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onMessageBoxClose(wb);}}/>
                <ContextMenu
                    contextId={'FileTreeContainer'}
                    menuId={'contextMenuForFiles'}
                    closeOnClickOut={true}
                    onOpen={(...args) => {return this.onFileContextOpen(...args);}}
                    onClose={() => {this.lockHoverChange = false; this.props.dispatch(setHoveredNode(undefined));}}
                    otherMenus={["contextMenuForDirectories"]}
                    items={[
                        {
                            label: this.props.dict.translate("$OpenFile", "file_tree"),
                            onClick: () => {
                                this.props.backend.workspace().loadFile(this.contextItem.key);
                            }
                        },
                        {
                            label: this.props.dict.translate("$OpenToTheSide", "file_tree"),
                            disabled: true,
                            onClick: () => {console.log('delete')},
                        },
                        {
                            line: true
                        },
                        {
                            label: this.props.dict.translate("$RenameFile", "file_tree"),
                            disabled: true,
                            onClick: () => {console.log('rename')},
                        },
                        {
                            line: true
                        },
                        {
                            label: this.props.dict.translate("$DeleteFile", "file_tree"),
                            onClick: () => {this.props.onDeleteFile(this.contextItem.key)},
                        }
                    ]}
                />
                <ContextMenu
                    contextId={'FileTreeContainer'}
                    menuId={'contextMenuForDirectories'}
                    closeOnClickOut={true}
                    onOpen={(...args) => {return this.onDirectoryContextOpen(...args);}}
                    onClose={() => {this.lockHoverChange = false; this.props.dispatch(setHoveredNode(undefined));}}
                    otherMenus={["contextMenuForFiles"]}
                    items={[
                        {
                            label: this.props.dict.translate("$SetAsActiveProject", "file_tree"),
                            onClick: (e) => {this.setActiveProject(e)}
                        }
                    ]}
                />
            </div>
        );
    }
}

export default connect(state => {
    return {
        fileTree: state.workspace.fileTree, 
        root: state.workspace.root,
        openFiles: state.openFiles.openFiles,
        backendState: state.backend,
        hoveredProps: state.workspace.hoveredNode
    }
})(FileView);