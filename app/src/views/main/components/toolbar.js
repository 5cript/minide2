import React from 'react';
import {connect} from 'react-redux';

// Styling
import { styled } from '@material-ui/core/styles';

// Components
import IconButton from '@material-ui/core/IconButton';
import Select from '@material-ui/core/Select';
import MenuItem from '@material-ui/core/MenuItem';
import InputLabel from '@material-ui/core/InputLabel';
import Input from '@material-ui/core/Input';
import { Combobox } from 'react-widgets';
import { MuiThemeProvider, createMuiTheme } from '@material-ui/core/styles';
import HamburgerMenu from 'react-hamburger-menu';
import ContextMenu from '../../../elements/context_menu';

// Other
import _ from 'lodash';

// Styles
import './styles/toolbar.css'

// Requires
//let dict = require('../../../util/localization');

const SimpleIconButton = styled(IconButton)({
    borderRadius: '0px',
    padding: '5px',
    marginLeft: '3px'
});

const StyledSelect = styled(Select)({
    color: 'var(--foreground-color)',
    '$ > ul': {
        paddingBottom: '0px'
    },
    borderRadius: '0px',
    underline: {
        borderBottom: '2px solid white',
        '&:after': {
            // The source seems to use this but it doesn't work
            borderBottom: '2px solid white',
        }
    }
})

const StyledLabel = styled(InputLabel)({
    color: 'var(--foreground-color)',
    fontSize: '9px'
})

const HoverFix = createMuiTheme({
    overrides: {
        MuiIconButton: {
            root: {
                '&:hover': {
                    backgroundColor: 'var(--background-color-brighter)'
                }
            }
        }
    }
})

class Toolbar extends React.Component {
    state = {
        shownBarId: '',
        openMenuId: null
    }

    constructor(props)
    {
        super(props);
    }

    buttonAction = (toolbar, item) => 
    {
        if (item.special_actions && item.special_actions.length > 0)
            console.log('SPECIAL_ACTIONS!!!')

        this.props.backend.toolbar().callAction(toolbar.id, item.id);
    }

    openMenu = (id) => 
    {
        if (this.state.openMenuId !== id)
            this.setState({openMenuId: id});
        else
            this.setState({openMenuId: null});
    }

    onMenuContextOpen(itemId, xOffset, yOffset, event)
    {
        const rect = event.target.getBoundingClientRect();
        let x = rect.left;
        x = x + (x%rect.width) - rect.width + 2;
        let y = rect.bottom + 4;
        y = y + (y%rect.height) - rect.height + 11;

        return {
            x: x,
            y: y,
            doShow: true
        }
    }

    onContextMenuItemClick = (toolbarId, itemId, event, label) => 
    {
        console.log(toolbarId, itemId, event, label)
    }

    buildToolbar = (id) => 
    {
        if (this.props.toolbars === undefined || _.isEmpty(this.props.toolbars))
            return <div />

        const toolbar = this.props.toolbars[id];
        if (toolbar === undefined || _.isEmpty(toolbar))
            return <div />

        let components = []
        for (let i in toolbar.items)
        {
            const item = toolbar.items[i]
            const mapper = item => 
            {
                switch(item.type)
                {
                    case("IconButton"):
                    {
                        const img = (item.pngbase64 && item.pngbase64.length > 0) 
                            ? 'data:image/png;base64, ' + item.pngbase64
                            : 'resources/images/toolbar/red_x.png'
                        ;
                        return <SimpleIconButton key={item.id} edge={false} onClick={() => this.buttonAction(toolbar, item)}>
                            <img alt={'ohno'} src={img}></img>
                        </SimpleIconButton>
                    }
                    case("Splitter"):
                    {
                        return <div key={item.id} className='Seperator' />
                    }
                    case("ComboBox"):
                    {
                        return <div key={item.id} className='ToolbarComboBoxDiv'>
                            <Combobox />
                        </div>
                    }
                    case("Menu"):
                    {
                        return (
                            <div 
                                key={item.id}
                                id={toolbar.id + "_" + item.id}
                                onClick={() => this.openMenu(item.id)}
                                className='MenuButton'
                            >
                                <HamburgerMenu
                                    isOpen={this.state.openMenuId === item.id}
                                    menuClicked={() => this.openMenu(item.id)}
                                    width={18}
                                    height={14}
                                    strokeWidth={2}
                                    rotate={0}
                                    color='white'
                                    borderRadius={0}
                                    animationDuration={0.3}
                                >
                                </HamburgerMenu>
                                <ContextMenu
                                    contextId={toolbar.id + "_" + item.id}
                                    menuId={toolbar.id + "_" + item.id + "_ctx"}
                                    closeOnClickOut={false}
                                    stayOpen={true}
                                    openOnClick={true}
                                    closeOnClick={true}
                                    onOpen={(...args) => {return this.onMenuContextOpen(item.id, ...args);}}
                                    otherMenus={[]}
                                    /*items={[
                                        {
                                            label: 'bob',
                                            onClick: (e, label) => {this.onContextMenuItemClick(toolbar.id, item.id, e, 'bob')}
                                        }
                                    ]}*/
                                    items={
                                        _.filter(item.entries.map(entry => 
                                        {
                                            const img = (entry.pngbase64 && entry.pngbase64.length > 0) 
                                                ? 'data:image/png;base64, ' + entry.pngbase64
                                                : undefined
                                            ;
                                            return {
                                                label: entry.label,
                                                line: entry.is_splitter,
                                                icon: img,
                                                onClick: () => {this.onContextMenuItemClick(toolbar.id, item.id, undefined, entry.label)}
                                            }
                                        }), item => {
                                            return item.label !== undefined || item.line === true;
                                        })
                                    }
                                ></ContextMenu>
                            </div>
                        )
                    }
                    default:
                        return <div key={item.id} >{item.id}</div>
                }
            }
            components.push(mapper(item));
        }
        return components
    }

    
    preselectToolbar = () => 
    {
        for (let i in this.props.toolbars)
        {
            this.setState({shownBarId: this.props.toolbars[i].id});
            break;
        }
    }

    render = () => {
        return (
            <div id='ToolbarContainer'>
                <div>
                    <StyledLabel id='toolbar-select-label'>Toolbar</StyledLabel>
                    <StyledSelect
                        labelId={'toolbar-select-label'}
                        value={this.state.shownBarId}
                        onChange={id => { this.setState({ shownBarId: id.target.value }) }}
                        input={<Input classes={{
                            underline: StyledSelect.underline,
                        }} />}
                    >
                    {
                        Object.values(this.props.toolbars).map(toolbar => {
                            return <MenuItem value={
                                toolbar.id
                            }>{toolbar.name}</MenuItem>
                        })
                    }
                    </StyledSelect>
                </div>
                <div className='Seperator' />
                <div id='ActualToolbar'>
                    <MuiThemeProvider theme={HoverFix}>
                    {
                        this.buildToolbar(this.state.shownBarId)
                    }
                    </MuiThemeProvider>
                </div>
            </div>
        )
    }
}

export default connect(state => {
    return {
        openFiles: state.openFiles.openFiles,
        activeFile: state.openFiles.activeFile,
        shortcuts: state.shortcuts,
        locale: state.locale,
        toolbars: state.toolbars.toolbars,
        lookup: state.toolbars.lookup
    }
}, null, null, {forwardRef: true})(Toolbar);