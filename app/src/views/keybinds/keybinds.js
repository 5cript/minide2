import React from 'react';
import {connect} from 'react-redux';

// Components
import {ShortcutRecorder, keybindRenderer} from '../../elements/shortcut_recorder';
import SearchableTable from '../../elements/searchable_table';
import {KeybindIo} from '../main/keybind_actor';
import ReactModal from '../../elements/modal';

// Other
import Dictionary from '../../util/localization';
import {setKeybind} from '../../actions/shortcut_actions';
import _ from 'lodash';

// Styles
import './styles/keybinds.css';

// Requires
const {ipcRenderer} = window.require('electron');

class Keybinds extends React.Component 
{
    dict = new Dictionary();

    state = {
        recorderVisible: false
    }

    clickedToChange = null;

    constructor(props)
    {
        super(props)
        
        this.header = 
        [
            {
                label: this.dict.translate('$Command', 'keybinds'),
                accessor: 'name'
            },
            {
                label: this.dict.translate('$Keybind', 'keybinds'),
                accessor: 'binding'
            }
        ]

        ipcRenderer.on('setHome', (event, arg) => {
            console.log(arg);
            this.home = arg;
            this.loadKeybinds();
        })
    }

    loadKeybinds = () => 
    {
        if (this.home)
        {
            this.keybindIo = new KeybindIo(this.home);
        }
    }

    saveKeybinds = (binds) => 
    {
        if (this.keybindIo)
            this.keybindIo.saveKeybindsToDrive(binds);
        else
            console.error('could not save');
    }

    elementsAsArray = () => 
    {
        const elements = this.props.shortcuts ? this.props.shortcuts : [];
        let result = []
        for (let keybind in elements) 
        {
            result.push({
                name: {value: this.dict.translate("$" + keybind, 'keybinds'), render: this.renderText},
                binding: {value: elements[keybind], render: this.renderKeybind},
                key: keybind
            })
        }
        return result;
    }

    filterElement = (elem, filterText) => 
    {
        return elem.name.value.toLowerCase().search(filterText.toLowerCase()) > -1;
    }

    changeKeybind = (value, index) => 
    {
        const filtered = this.table.filteredData();
        this.clickedToChange = filtered[index];
        this.showRecorder();
    }

    renderText = (value, index) => 
    {
        return <div 
            style={{
                backgroundColor: (index % 2) ? 'var(--background-color-darker)' : 'var(--background-color-brighter)',
                paddingLeft: '8px',
                paddingRight: '8px',
                paddingTop: '4px',
                paddingBottom: '4px'
            }}
            onClick={() => {this.changeKeybind(value, index)}}
        >{value}</div>
    }

    renderKeybind = (value, index) => 
    {
        return <div 
            style={{
                backgroundColor: (index % 2) ? 'var(--background-color-darker)' : 'var(--background-color-brighter)',
                paddingTop: '3px',
                paddingBottom: '3px'
            }}
            onClick={() => {this.changeKeybind(value, index)}}
        >{keybindRenderer(value)}</div>;
    }

    renderElement = (elem, index) => 
    {
        return elem.render(elem.value, index)
    }

    setSearchTable = (table) =>
    {
        this.table = table;
    }

    showRecorder = () => 
    {
        this.setState({recorderVisible: true});
    }

    onRecorderClose = () => 
    {
        this.setState({recorderVisible: false});
    }

    setNewKeybind = (keybind) => 
    {
        const keybindName = this.clickedToChange.key;
        const binding = {
			ctrl: keybind.ctrlKey,
			alt: keybind.altKey,
            shift: keybind.shiftKey,
            meta: keybind.metaKey,
			key: keybind.key,
			location: keybind.location
        };

        this.props.dispatch(setKeybind(keybindName, binding))
        let modded = _.cloneDeep(this.props.shortcuts);
        modded[keybindName] = binding;
        this.saveKeybinds(modded);
        this.onRecorderClose();
    }

    render = () => {
        return (
            <div>
                <SearchableTable
                    ref={this.setSearchTable}
                    elements={this.elementsAsArray()}
                    tableHeader={this.header}
                    elementRenderer={this.renderElement}
                    elementFilter={this.filterElement}
                    elementKey={elem => elem.name}
                    dict={this.dict}
                ></SearchableTable>
                <ReactModal initWidth={this.props.width ? this.props.width : 500} initHeight={this.props.height ? this.props.height : 200}
                    onFocus={() => { }}
                    onRequestClose={() => { this.onRecorderClose() }}
                    isOpen={this.state.recorderVisible}
                >
                    <ShortcutRecorder
                        className={"shortcutRecorderModal"}
                        dict={this.dict}
                        onClose={this.onRecorderClose}
                        onAccept={this.setNewKeybind}
                        shortcutName={this.clickedToChange ? this.clickedToChange.name.value : ''}
                    ></ShortcutRecorder>
                </ReactModal>
            </div>
        )
    }
}

export default connect(state => {
    return {
        shortcuts: state.shortcuts.bindings
    }
})(Keybinds);