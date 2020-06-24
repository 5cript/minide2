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
        shownBarId: ''
    }

    constructor(props)
    {
        super(props);
        console.log(this.props.toolbars)
    }

    buttonAction = (toolbar, item) => 
    {
        if (item.special_actions && item.special_actions.length > 0)
            console.log('SPECIAL_ACTIONS!!!')

        this.props.backend.toolbar().callAction(toolbar.id, item.id);
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
                        return <div className='ToolbarComboBoxDiv'>
                            <Combobox key={item.id} />
                        </div>
                    }
                    default:
                        return <div key={item.id} >{item.id}</div>
                }
            }
            components.push(mapper(item));
        }
        console.log(components)
        return components
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
        /*
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
                        <MenuItem value={0}>CMake C/C++</MenuItem>
                        <MenuItem value={1}>Bash</MenuItem>
                    </StyledSelect>
                </div>
                <div className='Seperator' />
                <div id='ActualToolbar'>
                    <MuiThemeProvider theme={HoverFix}>
                    {ComponentSwitch(this.state.shownBarId,
                        <div className='IconButtonGroup'>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onSave}>
                                <img alt={'ohno'} src={'resources/images/toolbar/save.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onSaveAll}>
                                <img alt={'ohno'} src={'resources/images/toolbar/save_all.png'}></img>
                            </SimpleIconButton>
                            <div className='Seperator' />
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onCmake}>
                                <img alt={'ohno'} src={'resources/images/toolbar/cmake.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onBuild}>
                                <img alt={'ohno'} src={'resources/images/toolbar/build.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onRun}>
                                <img alt={'ohno'} src={'resources/images/toolbar/run.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onBuildRun}>
                                <img alt={'ohno'} src={'resources/images/toolbar/build_run.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onCancel}>
                                <img alt={'ohno'} src={'resources/images/toolbar/red_x.png'}></img>
                            </SimpleIconButton>
                            <div className='Seperator' />
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onDebug}>
                                <img alt={'ohno'} src={'resources/images/toolbar/debug.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onNextLine}>
                                <img alt={'ohno'} src={'resources/images/toolbar/next_line.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onStepInto}>
                                <img alt={'ohno'} src={'resources/images/toolbar/step_into.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onStepOut}>
                                <img alt={'ohno'} src={'resources/images/toolbar/step_out.png'}></img>
                            </SimpleIconButton>
                        </div>,
                        <div className='IconButtonGroup'>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onSave}>
                                <img alt={'ohno'} src={'resources/images/toolbar/save.png'}></img>
                            </SimpleIconButton>
                            <SimpleIconButton edge={false} onClick={this.props.cmake.onSaveAll}>
                                <img alt={'ohno'} src={'resources/images/toolbar/save_all.png'}></img>
                            </SimpleIconButton>
                        </div>
                    )}
                    </MuiThemeProvider>
                </div>
            </div>
        )
        */
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
})(Toolbar);