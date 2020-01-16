import React, {PureComponent} from 'react';
import {Treebeard} from 'react-treebeard';

import {connect} from 'react-redux';

// Styles
import './styles/file_tree.css';

const data = {
    name: 'root',
    toggled: true,
    children: [
        {
            name: 'parent',
            children: [
                { name: 'child1' },
                { name: 'child2' }
            ]
        },
        {
            name: 'loading parent',
            loading: true,
            children: []
        },
        {
            name: 'parent',
            children: [
                {
                    name: 'nested parent',
                    children: [
                        { name: 'nested child 1' },
                        { name: 'nested child 2' }
                    ]
                }
            ]
        }
    ]
}

class FileView extends PureComponent {
    constructor(props){
        super(props);
        this.state = {data};
        this.onToggle = this.onToggle.bind(this);

        this.fileList = props.fileList;
    }
    
    onToggle(node, toggled){
        const {cursor, data} = this.state;
        if (cursor) {
            this.setState(() => ({cursor, active: false}));
        }
        node.active = true;
        if (node.children) { 
            node.toggled = toggled; 
        }
        this.setState(() => ({cursor: node, data: Object.assign({}, data)}));
    }
    
    render(){
        const {data} = this.state;
        return (
            <>
                <Treebeard
                    data={data}
                    onToggle={this.onToggle}
                />
                <code>
                    {JSON.stringify(this.props.fileTree)}
                </code>
            </>
        );
    }
}

export default connect(state => {
    return state.workspace.fileTree
})(FileView);