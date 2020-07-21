import React from 'react';
import {connect} from 'react-redux';

// Components

// Actions
import {activeFileWasSynchronized, fileWasSynchronized, setActiveFile, removeOpenFile} from '../../actions/open_file_actions';
import {setAllKeybinds} from '../../actions/shortcut_actions';

// Other
import _ from 'lodash';

// requires
const fs = window.require('fs');

class KeybindActor extends React.Component 
{
    isShortcut = (event, shortcutDefinition) =>
    {
        if (shortcutDefinition === undefined)
        {
            // there is no binding for this.
            return;
        }

        if (event.type !== 'keyup')
            return;
            
        const moddedDefinition = {
            key: shortcutDefinition.key,
            ctrlKey: shortcutDefinition.ctrl,
            altKey: shortcutDefinition.alt,
            shiftKey: shortcutDefinition.shift
        };

        let is = true;
        _.forIn(moddedDefinition, (v, k) => 
        {
            if (k === 'key')
            {
                if (event[k].toLowerCase() !== v.toLowerCase())
                {
                    is = false;
                    return false;
                }
            }
            else if (v !== undefined && event[k] !== v)
            {
                is = false;
                return false;
            }
        });
        return is;
    }

    loadKeybindsFromDrive = (home) =>
    {
        const keybinds = fs.readFileSync(home + "/keybinds.json", {encoding: 'utf-8'});
        if (keybinds === null || keybinds === undefined || keybinds === '')
        {
            console.error('no keybinds');
            return;
        }

        try
        {
            this.props.dispatch(setAllKeybinds(JSON.parse(keybinds).bindings))
        }
        catch(exc)
        {
            console.error(exc);
        }
    }

    toggleSourceHeader = () => 
    {
        if (this.props.activeFile === -1)
            return;
        this.props.backend.workspace().toggleSourceHeader(this.props.openFiles[this.props.activeFile].path);
    }

    advanceEditorTab = (amount) => 
    {
        const openFileCount = this.props.openFiles.length;
        if (openFileCount === 0)
            return;
        if (openFileCount === 1)
            return this.props.dispatch(setActiveFile(0))
        
        let newActive = this.props.activeFile + amount;
        if (newActive >= openFileCount)
            newActive = 0;
        if (newActive < 0)
            newActive = openFileCount - 1;

        this.props.dispatch(setActiveFile(newActive));
    }

    closeActiveFile = () => 
    {
        if (this.props.activeFile !== -1)
        {
            const file = this.props.openFiles[this.props.activeFile];
            if (!file.synchronized) 
            {
                this.props.mainWindow.showYesNoBox(this.props.dict.translate("$CloseUnsavedWarning", "dialog"), () => {
                    this.props.dispatch(removeOpenFile(file.path));     
                })
            }
            else
                this.props.dispatch(removeOpenFile(file.path)); 
        }
    }

    onKey = (event) =>
    {
        const pbindings = this.props.shortcuts.bindings;
        const bindings = [
            {combination: pbindings.save, action: ()=>{this.props.commonActions.saveFile();}},
            {combination: pbindings.saveAll, action: ()=>{this.props.commonActions.saveAllFiles()}},
            {combination: pbindings.toggleSourceHeader, action: ()=>{this.toggleSourceHeader()}},
            {combination: pbindings.editorTabPrevious, action: ()=>{this.advanceEditorTab(-1)}},
            {combination: pbindings.editorTabNext, action: ()=>{this.advanceEditorTab(1)}},
            {combination: pbindings.closeActiveFile, action: ()=>{this.closeActiveFile()}}
        ];

        for (let binding of bindings)
        {
            if (this.isShortcut(event, binding.combination))
            {
                binding.action();
                return;
            }
        }
    }

    render()
    {return(
        <div
            style={{display: 'none'}}
        ></div>
    )}
}

export default connect(state => {
    return {
        openFiles: state.openFiles.openFiles,
        activeFile: state.openFiles.activeFile,
        shortcuts: state.shortcuts
    }
}, null, null, {forwardRef: true})(KeybindActor);