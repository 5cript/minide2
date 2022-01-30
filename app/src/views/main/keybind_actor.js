import React from 'react';
import {connect} from 'react-redux';

// Components

// Actions
import {setActiveFile, removeOpenFile} from '../../actions/open_file_actions';
import {setAllKeybinds} from '../../actions/shortcut_actions';

// Other
import _ from 'lodash';

// requires
const fs = window.require('fs');

export class KeybindIo
{
    constructor(home)
    {
        this.home = home;
    }


    loadKeybindsFromDrive = () =>
    {
        const keybinds = fs.readFileSync(this.home + "/keybinds.json", {encoding: 'utf-8'});
        if (keybinds === null || keybinds === undefined || keybinds === '')
        {
            console.error('no keybinds');
            return {};
        }

        const defaultTo = () => 
        {
            const def = this.makeDefault();
            this.saveKeybindsToDrive(def);
            return def;
        }
        try
        {
            const file = JSON.parse(keybinds);
            if (file === undefined || file.bindings === undefined)
                return defaultTo();
            return file.bindings;
        }
        catch(exc)
        {
            console.error(exc);
            return defaultTo();
        }
    }

    saveKeybindsToDrive = (bindings) => 
    {
        fs.writeFileSync(
            this.home + "/keybinds.json", 
            JSON.stringify({bindings: bindings}, null, 4),
            {encoding: 'utf-8'}
        );
    }

    makeDefault = () =>
    {
        return {
            save: {
                key: 's',
                ctrl: true
            },
            saveAll: {
                key: 's',
                ctrl: true,
                shift: true
            },
            toggleSourceHeader: {
                key: 'F11'
            },
            editorTabPrevious: {
                key: 'tab',
                ctrl: true,
                shift: true
            },
            editorTabNext: {
                key: 'tab',
                ctrl: true
            },
            closeActiveFile: {
                key: 'w',
                ctrl: true
            }
        }
    }
}

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
            shiftKey: shortcutDefinition.shift,
            location: shortcutDefinition.location
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
        this.home = home;
        const io = new KeybindIo(this.home);
        this.props.dispatch(setAllKeybinds(io.loadKeybindsFromDrive(home)))
    }

    saveKeybinds = () => 
    {
        const io = new KeybindIo(this.home);
        io.saveKeybindsToDrive(this.props.shortcuts.bindings);
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