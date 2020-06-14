import React from 'react';

import _ from 'lodash';
import classNames from 'classnames';

import './styles/editable_list.css';
import './styles/various_inputs.css';

class EditableList extends React.Component
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
            <div className={classNames("editableListBox", this.props.className)}>
                {(()=>
                {
                    let inc = -1;
                    return (
                        this.props.items.map(item => {
                            let localInc = _.clone(++inc);
                            if (this.props.onChange === undefined || inc !== this.state.editingIndex)
                                return <div style={{display: "flex"}}>
                                    <div key={inc} onDoubleClick={() => {this.setState({editingIndex: localInc});}}>{item}</div>
                                    <div className="removeButton" onClick={e => this.props.onRemove(localInc)}>{"\u274C"}</div>
                                </div>
                            else
                                return (
                                    <input 
                                        className="sneakInput" 
                                        ref={this.setInputRef} 
                                        key={inc} 
                                        autoFocus
                                        value={item === undefined ? '' : item} 
                                        onChange={
                                            (e) => {this.props.onChange(localInc, e.target.value)}
                                        }
                                        onKeyPress={
                                            (e) => {if (e.key === "Enter") this.defocus();}
                                        }
                                        onFocus={e => {
                                            let val = e.target.value;
                                            e.target.value = '';
                                            e.target.value = val;
                                        }}
                                    />
                                );
                        })
                    );
                }
                )()}
            </div>
        );
    }
}

export default EditableList;