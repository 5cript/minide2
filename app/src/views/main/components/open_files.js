import React from 'react';

import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';

//import { observable, computed, action, decorate } from "mobx"

class OpenFilesList extends React.Component
{
    constructor(props) {
        super(props);
        this.openFiles = this.props.openFiles;
    }

    render = () => {
        return (
            <div>
                <List>
                    <ListItem>hi</ListItem>
                </List>
            </div>
        )
    }
}


export default OpenFilesList;