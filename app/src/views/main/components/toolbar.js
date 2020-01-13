import React from 'react';

// Styling
import { styled } from '@material-ui/core/styles';

// Components
import IconButton from '@material-ui/core/IconButton';
import Select from '@material-ui/core/Select';
import MenuItem from '@material-ui/core/MenuItem';
import InputLabel from '@material-ui/core/InputLabel';
import Input from '@material-ui/core/Input';
import { MuiThemeProvider, createMuiTheme } from '@material-ui/core/styles';

// Other
import ComponentSwitch from '../../../util/switch';

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
        shownBarId: 0
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
    }
}

export default Toolbar;