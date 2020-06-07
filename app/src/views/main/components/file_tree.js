import React, {PureComponent} from 'react';
//import {Treebeard} from 'react-treebeard';
import Tree from 'rc-tree';

import {connect} from 'react-redux';
import classNames from 'classnames';

import _ from 'lodash';

//import { setActiveFile, removeOpenFile } from '../../../actions/open_file_actions';
import * as openFileActions from '../../../actions/open_file_actions';

// Styles
import './styles/file_tree.css';

const Icon = ({ selected }) => (
    <div className={classNames('customized-icon')}> </div>
);
  
/*
const treeData = [
    {
      key: '0-0',
      title: 'parent 1',
      children: [
        { key: '0-0-0', title: 'parent 1-1', children: [{ key: '0-0-0-0', title: 'parent 1-1-0' }] },
        {
          key: '0-0-1',
          title: 'parent 1-2',
          children: [
            { key: '0-0-1-0', title: 'parent 1-2-0', disableCheckbox: true },
            { key: '0-0-1-1', title: 'parent 1-2-1' },
            { key: '0-0-1-2', title: 'parent 1-2-2' },
            { key: '0-0-1-3', title: 'parent 1-2-3' },
            { key: '0-0-1-4', title: 'parent 1-2-4' },
            { key: '0-0-1-5', title: 'parent 1-2-5' },
            { key: '0-0-1-6', title: 'parent 1-2-6' },
            { key: '0-0-1-7', title: 'parent 1-2-7', children:[{ key: 1123, title: 1123 }] },
            { key: '0-0-1-8', title: 'parent 1-2-8' },
            { key: '0-0-1-9', title: 'parent 1-2-9' },
            { key: 1128, title: 1128 },
          ],
        },
      ],
    },
  ];
*/

class FileView extends PureComponent {
    constructor(props){
        super(props);

        this.treeRef = React.createRef();
    }

    setTreeRef = tree => {
         this.tree = tree;
    };


    onExpand = expandedKeys => {
        let path = _.difference(expandedKeys, this.expandedKeys);
        this.expandedKeys = expandedKeys;

        if (path.length === 0)
            return;

        if (path[0] !== this.props.root)
            this.props.backend.workspace().enumDirectory(path[0]);
    };

    onTreeRightClick = event => {
        console.log('right click: ' + event.node.key);
    }

    onNodeDoubleClick = (event) => {
        if (this.hoveredNode === undefined)
            return;

        this.props.backend.workspace().loadFile(this.hoveredNode.key);

        //his.props.dispatch(openFileActions.addOpenFile(this.hoveredNode.key));
    }

    onMouseEnter = (v) => {
        this.hoveredNode = v.node;
    }
    
    render(){
        return (
            <>
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
            </>
        );
    }
}

export default connect(state => {
    return {fileTree: state.workspace.fileTree, root: state.workspace.root}
})(FileView);