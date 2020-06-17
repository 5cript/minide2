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

    isEnabled = () => 
    {
        return this.props.enabled !== false;
    }

    isDisabled = () => 
    {
        return !this.isEnabled();
    }

    render()
    {
        return (
            <div className={classNames("editableListBox", this.props.className, !this.isEnabled() ? 'disabledListBox' : '')}>
                {(()=>
                {
                    let inc = -1;
                    return (
                        this.props.items.map(item => {
                            let localInc = _.clone(++inc);
                            if (this.props.onChange === undefined || inc !== this.state.editingIndex)
                                return <div key={inc} style={{display: "flex"}}>
                                    <div 
                                        className={classNames(!this.isEnabled() ? 'disabledListItem' : '')}
                                        onDoubleClick={() => {
                                            if (this.isDisabled())
                                                return;
                                            this.setState({editingIndex: localInc});
                                        }
                                    }>{item}</div>
                                    <div className="listRemoveButton" onClick={e => {
                                        if (this.isDisabled())
                                            return;
                                        this.props.onRemove(localInc)
                                    }}>{"\u274C"}</div>
                                </div>
                            else
                                return (
                                    <input 
                                        className={classNames("sneakInput", "editableListInput")}
                                        ref={this.setInputRef} 
                                        key={inc} 
                                        autoFocus
                                        value={item === undefined ? '' : item} 
                                        onChange={e => {
                                            if (this.isDisabled())
                                                return;
                                            this.props.onChange(localInc, e.target.value)
                                        }}
                                        onKeyPress={e => {
                                            if (this.isDisabled())
                                                return;
                                            if (e.key === "Enter") this.defocus();
                                        }}
                                        onFocus={e => {
                                            if (this.isDisabled())
                                                return;
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