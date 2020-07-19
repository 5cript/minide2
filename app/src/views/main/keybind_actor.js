import React from 'react';
import {connect} from 'react-redux';

// Components

// Actions
import {activeFileWasSynchronized, fileWasSynchronized} from '../../actions/open_file_actions';
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
            console.error('oups, shortcut is not defined?! look at shortcut store');

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
            else if (event[k] !== v)
            {
                is = false;
                return false;
            }
        });
        return is;
    }

    saveFile = () =>
    {
        if (this.props.activeFile >= 0) 
        {
            let file = this.props.openFiles[this.props.activeFile];
            if (file.isAbsolutePath)
            {
                this.props.mainWindow.showYesNoBox(this.dict.translate("$FileOutsideWorkspace", "dialog"), () => {
                    this.props.backend.workspace().saveFile(file.path, file.content, () => 
                    {
                        this.props.dispatch(activeFileWasSynchronized());
                    });
                })
            }
            else
            {
                this.props.backend.workspace().saveFile(file.path, file.content, () => 
                {
                    this.props.dispatch(activeFileWasSynchronized());
                });
            }
        }
        else
        {
            this.props.mainWindow.showOkBox('todo: implement save as for void model');
            return;
        }
    }

    saveAllFiles = () =>
    {
        this.props.openFiles.map(file => 
        {
            if (!file.synchronized)
            {
                this.props.backend.workspace().saveFile(file.path, file.content, () => 
                {
                    this.props.dispatch(fileWasSynchronized(file.path));
                });
            }
            return file;
        })
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

    onKey = (event) =>
    {
        const pbindings = this.props.shortcuts.bindings;
        const bindings = [
            {combination: pbindings.save, action: ()=>{this.saveFile()}},
            {combination: pbindings.saveAll, action: ()=>{this.saveAllFiles()}},
            {combination: pbindings.toggleSourceHeader, action: ()=>{this.toggleSourceHeader()}}
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