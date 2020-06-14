import React from 'react';

import _ from 'lodash';
import classNames from 'classnames';

import './styles/editable_table.css';
import './styles/various_inputs.css';

class EditableTable extends React.Component
{
    state=
    {
        editingIndex: -1
    }

    constructor(props)
    {
        super(props);

        document.addEventListener('mousedown', this.defocus);
    }

    defocus = (event) =>
    {
        if (event === undefined || (this.inputRef && !this.inputRef.contains(event.target)))
            this.setState({editingIndex: -1});
    }

    setInputRef = (node) => 
    {
        this.inputRef = node;
    }

    render()
    {
        return (
            <div className={classNames("editableTable", this.props.className)}>
            </div>
        );
    }
}

export default EditableTable;