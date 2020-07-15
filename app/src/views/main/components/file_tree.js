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
import {setActiveProject} from '../../../actions/workspace_actions';

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

        const nodeKey = _.clone(this.hoveredNode.key);

        let file = this.props.openFiles.find(file => file.path === this.hoveredNode.key);
        if (file === undefined)
            this.props.backend.workspace().loadFile(this.hoveredNode.key);
        else
        {
            this.showYesNoBox(this.props.dict.translate("$ReloadFileFromServer", "dialog"), () => {
                this.props.backend.workspace().loadFile(nodeKey);
            });
        }
    }

    onMouseEnter(v)
    {
        this.hoveredNode = v.node;
    }

    onMouseLeave(v)
    {
        this.hoveredNode = undefined;
    }

    onFileContextOpen(xOffset, yOffset, event)
    {
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

        this.props.backend.workspace().setActiveProject(contextItem.key, () => {
            this.props.persistence.setLastActive(this.currentHost(), contextItem.key);
            this.props.onActiveProjectSet(contextItem.key);
            this.props.dispatch(setActiveProject(contextItem.key))
        });
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
                            <div style={{background: "var(--background-color)"}}>
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
                    otherMenus={["contextMenuForDirectories"]}
                    items={[
                        {
                            label: this.props.dict.translate("$OpenFile", "file_tree"),
                            onClick: () => {console.log('configure')}
                        },
                        {
                            label: this.props.dict.translate("$OpenToTheSide", "file_tree"),
                            onClick: () => {console.log('delete')},
                        },
                        {
                            line: true
                        },
                        {
                            label: this.props.dict.translate("$RenameFile", "file_tree"),
                        },
                        {
                            line: true
                        },
                        {
                            label: this.props.dict.translate("$DeleteFile", "file_tree"),
                            onClick: () => {console.log('delete')},
                        }
                    ]}
                />
                <ContextMenu
                    contextId={'FileTreeContainer'}
                    menuId={'contextMenuForDirectories'}
                    closeOnClickOut={true}
                    onOpen={(...args) => {return this.onDirectoryContextOpen(...args);}}
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
        backendState: state.backend
    }
})(FileView);