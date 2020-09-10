import React from 'react';
import {connect} from 'react-redux';

// Components
import DropdownList from 'react-widgets/lib/DropdownList';
import {FormGroup, FormLabel, FormControlLabel} from '@material-ui/core';
import {DragDropContext} from 'react-beautiful-dnd';
import SlimButton from '../../elements/button';
import {MuiTabs, TabPanel} from '../../elements/tabs';
import ThemedSwitch from '../../elements/switch';
import InputBox from '../../elements/input_box';
import MessageBox from '../../elements/message_box';

// Other
import Dictionary from '../../util/localization';
import classNames from 'classnames';
import _ from 'lodash';

// Style
import './styles/debugger_settings.css';
import '../../elements/styles/various_inputs.css';

function MinimalInput({className, ...otherProps}) {
    return <input {...otherProps} className={classNames("styledInput", className)}></input>
}

class DebuggerSettings extends React.Component
{
    dict = new Dictionary();

    debuggers = ['gdb', 'lldb']

    state = {
        activeTab: 0,
        userProfiles: ['dummy'], // todo move to redux
        selectedDebugger: '',
        selectedProfile: '',
        profileInputVisible: false,
        yesNoBoxVisible: false,
        yesNoMessage: 'You should never see this',
        okBoxVisible: false,
        okMessage: 'You should never see this',

		buildBeforeDebug: false,
		runBinary: false,
		watchLocalVariables: false,
		watchFuncArgs: false,
		fullyReadSymb: false,
		neverReadSymb: false,
		pipeChildOutput: false,
		useInitCmdFile: false,
		useCmdFile: false,
		ignoreGdbInit: false,
		ignoreAllGdbInit: false,
        EnvrOverWrtTrgtEnvr: false,
        useAdditionalArgs: false
    }

    dndDragEnd = (dndOperation) =>
    {
    }

    
    getItemStyle = (isDragging, isSelected, draggableStyle) => ({
        backgroundColor: isDragging ? 'transparent' : (isSelected ? 'var(--background-color-very-bright)' : 'var(--background-color-darker'),
        color: isSelected ? 'var(--theme-color-brighter)' : undefined,
        height: '35px',
        paddingTop: '7px',
        paddingLeft: '8px',
        paddingRight: '8px',
    
        // styles we need to apply on draggables
        ...draggableStyle,
    });

    validProfileSelected() 
    {
        return this.state.selectedProfile !== '' && this.state.selectedProfile !== null && this.state.selectedProfile !== undefined;
    }

    setInputBoxRef = (node) => 
    {
        this.inputBox = node;
    }

    showOkBox(message, okAction) 
    {
        this.setState({
            okBoxVisible: true,
            okMessage: message
        })
        this.okAction = okAction;
    }

    onOkBoxClose(whatButton)
    {
        this.setState({
            okBoxVisible: false
        });
        if (whatButton === "Ok" && this.okAction !== undefined)
            this.okAction();
    }

    showYesNoBox(message, yesAction, noAction) 
    {
        this.setState({
            yesNoBoxVisible: true,
            yesNoMessage: message
        })
        this.yesAction = yesAction;
        this.noAction = noAction;
    }

    onYesNoBoxClose(whatButton)
    {
        this.setState({
            yesNoBoxVisible: false
        });
        if (whatButton === "Yes" && this.yesAction !== undefined)
            this.yesAction();
        if (whatButton === "No" && this.noAction !== undefined)
            this.noAction();
    }

    onProfileFormButtonPress(whatButton, input)
    {
        this.setState({profileInputVisible: false});

        if (whatButton === "Cancel")
            return;

        const i = input.find(e => e.key === 'profile')
        const profileName = i.value;

        if (this.state.userProfiles.findIndex(elem => elem === profileName) !== -1)
        {
            this.showOkBox(this.dict.translate('$ProfileAlreadyExists', 'debugger_settings'));
            this.inputBox.reset();
            return;
        }

        let profiles = _.cloneDeep(this.state.userProfiles);
        profiles.push(profileName);

        this.setState({
            userProfiles: profiles,
            selectedProfile: profileName
        })

        this.inputBox.reset();
    }

    deleteProfile = () => 
    {
        let reallyDelete = () => 
        {
            let profiles = _.cloneDeep(this.state.userProfiles);
            let profileIndex = profiles.findIndex(elem => elem === this.state.selectedProfile);
            if (profileIndex === -1)
                return;

            profiles.splice(profileIndex, 1);
            this.setState({
                userProfiles: profiles,
                selectedProfile: undefined
            });            
        };

        if (this.validProfileSelected())
            this.showYesNoBox(this.dict.translate('$ConfirmProfileRemove', 'debugger_settings'), reallyDelete)
    }

    render() {
        // NOTE TO MY FUTURE SELF
        // Allow X amount of debugger configuration and settings
        // give these configs a name that can be accessed from the run config like "${DebuggerProfile:lldb}" if there is a profile called lldb.
        // before creating a new debugger instance, these profiles (the settings) could then be merged into the run config
        // for the server to read.
        // make a selection box for what debugger it is (gdb, lldb).

        // maybe something like:
        // https://cdn.discordapp.com/attachments/745676647184990329/752688417284030544/unknown.png
        return (
            <div>
                <DragDropContext
                    onDragEnd={(...args) => {
                        this.dndDragEnd(...args)
                    }}
                >
                    <MuiTabs
                        onChange={(tabIndex) => {this.setState({activeTab: tabIndex})}}
                        value={this.state.activeTab}
                        tabLabels={[
                            this.dict.translate('$General', 'debugger_settings'),
                            this.dict.translate("$Profiles", 'debugger_settings')
                        ]}
                        id={'debuggerMainTabs'}
                        headerClass={'debuggerSettingsTabBox'}
                        leanTabsClass={'debuggerSettingsTabs'}
                        itemStyler={this.getItemStyle}
                    >
                        <TabPanel 
                            index={0}
                            value={this.state.activeTab}
                        >
                            <div id="generalSettings">
                                <FormLabel component="legend"></FormLabel>
                                <FormGroup>
                                    <FormControlLabel
                                        control={<ThemedSwitch checked={this.state.buildBeforeDebug} onChange={val => this.setState({buildBeforeDebug: val.target.checked})} name="buildBeforeDebug" />}
                                        label={this.dict.translate("$BuildBeforeDebug", 'debugger_settings')}
                                    />
                                    <FormControlLabel
                                        control={<ThemedSwitch checked={this.state.runBinary} onChange={val => this.setState({runBinary: val.target.checked})} name="runBinary" />}
                                        label={this.dict.translate("$ImmediatelyRunBinary", 'debugger_settings')}
                                    />
                                    <FormControlLabel
                                        control={<ThemedSwitch checked={this.state.watchLocalVariables} onChange={val => this.setState({watchLocalVariables: val.target.checked})} name="watchLocalVariables" />}
                                        label={this.dict.translate("$WatchLocalVariables", 'debugger_settings')}
                                    />
                                    <FormControlLabel
                                        control={<ThemedSwitch checked={this.state.watchFuncArgs} onChange={val => this.setState({watchFuncArgs: val.target.checked})} name="watchFuncArgs" />}
                                        label={this.dict.translate("$WatchFunctionArguments", 'debugger_settings')}
                                    />
                                </FormGroup>
                            </div>
                        </TabPanel>
                        <TabPanel 
                            index={1}
                            value={this.state.activeTab}
                        >                        
                            <div id="debuggerSettingsProfileSelectBox">
                                <label id="debuggerSettingsProfileLabel">{this.dict.translate("$Profile", 'debugger_settings') + ":"}</label>
                                <DropdownList
                                    id="debuggerSettingsProfileDropdown"
                                    data={this.state.userProfiles}
                                    onChange={value => this.setState({selectedProfile: value})}
                                    value={this.state.selectedProfile}
                                />
                                <div id="debuggerProfileSelectButtons">
                                    <SlimButton 
                                        onClick={x => this.setState({profileInputVisible: true})} 
                                        className="debuggerProfileSelectButton">{this.dict.translate("$Add", 'debugger_settings')}
                                    </SlimButton>
                                    <SlimButton 
                                        onClick={x => {this.deleteProfile()}}
                                        className="debuggerProfileSelectButton">{this.dict.translate("$Delete", 'debugger_settings')}
                                    </SlimButton>
                                </div>
                            </div>
                            <div id="debuggerProfiles" style={{
                                pointerEvents: this.validProfileSelected() ? undefined : 'none',
                                opacity: this.validProfileSelected() ? undefined : '0.5'
                            }}>
                                <div id="debuggerSelect">
                                    <label id="debuggerSettingsDebuggerLabel">{this.dict.translate("$Debugger", 'debugger_settings') + ":"}</label>
                                    <div>
                                        <DropdownList
                                            data={this.debuggers}
                                            onChange={value => this.setState({selectedDebugger: value})}
                                            value={this.state.selectedDebugger}
                                        />
                                    </div>
                                </div>
                                <div id="gdbSettings" style={{display: this.state.selectedDebugger !== 'gdb' ? 'none' : undefined}}>
                                    <div
                                        className="debuggerPathInputFrame"
                                    >
                                        <label>{this.dict.translate("$Path", 'debugger_settings') + ":"}</label>
                                        <MinimalInput className="debuggerFormInput"/>
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.state.fullyReadSymb} onChange={val => this.setState({fullyReadSymb: val.target.checked})} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$FullyReadSymbols", 'debugger_settings')}</label>
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.state.neverReadSymb} onChange={val => this.setState({neverReadSymb: val.target.checked})} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$NeverReadSymbols", 'debugger_settings')}</label>
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.state.pipeChildOutput} onChange={val => this.setState({pipeChildOutput: val.target.checked})} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$OutputChildResult", 'debugger_settings')}</label>
                                    </div>
                                    <div className="debuggerSettingsWithInput">
                                        <div>
                                            <ThemedSwitch checked={this.state.useInitCmdFile} onChange={val => this.setState({useInitCmdFile: val.target.checked})} />
                                            <label className="debuggerSwitchLabel">{this.dict.translate("$InitCommandFile", 'debugger_settings') + ":"}</label>
                                        </div>
                                        <MinimalInput 
                                            className={classNames("debuggerFormInput", "debuggerAdditionalInput", this.state.useInitCmdFile ? undefined : 'debuggerDisabledInput')} 
                                            type="text" 
                                            disabled={!this.state.useInitCmdFile} 
                                        />
                                    </div>
                                    <div className="debuggerSettingsWithInput">
                                        <div>
                                            <ThemedSwitch checked={this.state.useCmdFile} onChange={val => this.setState({useCmdFile: val.target.checked})} />
                                            <label className="debuggerSwitchLabel">{this.dict.translate("$CommandFile", 'debugger_settings') + ":"}</label>
                                        </div>
                                        <MinimalInput 
                                            className={classNames("debuggerFormInput", "debuggerAdditionalInput", this.state.useCmdFile ? undefined : 'debuggerDisabledInput')} 
                                            type="text" 
                                            disabled={!this.state.useCmdFile} 
                                        />
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.state.ignoreGdbInit} onChange={val => this.setState({ignoreGdbInit: val.target.checked})} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$Ignore", 'debugger_settings') + " ~/.gdbinit"}</label>
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.state.ignoreAllGdbInit} onChange={val => this.setState({ignoreAllGdbInit: val.target.checked})} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$IgnoreAll", 'debugger_settings') + " .gdbinit"}</label>
                                    </div>
                                    <div className="debuggerSettingsWithInput">
                                        <div>
                                            <ThemedSwitch checked={this.state.useAdditionalArgs} onChange={val => this.setState({useAdditionalArgs: val.target.checked})} />
                                            <label className="debuggerSwitchLabel">{this.dict.translate("$AdditionalCommandline", 'debugger_settings')}</label>
                                        </div>
                                        <MinimalInput 
                                            className={classNames("debuggerFormInput", "debuggerAdditionalInput", this.state.useAdditionalArgs ? undefined : 'debuggerDisabledInput')} 
                                            type="text" 
                                            disabled={!this.state.useAdditionalArgs} 
                                        />
                                    </div>
                                </div>
                                <div id="lldbSettings" style={{display: this.state.selectedDebugger !== 'lldb' ? 'none' : undefined}}>
                                    <div
                                        className="debuggerPathInputFrame"
                                    >
                                        <label>{this.dict.translate("$Path", 'debugger_settings') + ":"}</label>
                                        <MinimalInput className="debuggerFormInput"/>
                                    </div>
                                    <div className="debuggerSettingsWithInput">
                                        <div>
                                            <ThemedSwitch checked={this.state.useAdditionalArgs} onChange={val => this.setState({useAdditionalArgs: val.target.checked})} />
                                            <label className="debuggerSwitchLabel">{this.dict.translate("$AdditionalCommandline", 'debugger_settings')}</label>
                                        </div>
                                        <MinimalInput 
                                            className={classNames("debuggerFormInput", "debuggerAdditionalInput", this.state.useAdditionalArgs ? undefined : 'debuggerDisabledInput')} 
                                            type="text" 
                                            disabled={!this.state.useAdditionalArgs} 
                                        />
                                    </div>
                                </div>
                            </div>
                        </TabPanel>
                    </MuiTabs>
                </DragDropContext>

                <InputBox 
                    dict={this.dict} 
                    ref={this.setInputBoxRef}
                    visible={this.state.profileInputVisible} 
                    message={'boop'} 
                    onButtonPress={(...args)=>{this.onProfileFormButtonPress(...args);}}
                    schema={{
                        fields: [
                            {
                                key: 'profile',
                                label: this.dict.translate('$ProfileName', 'debugger_settings'),
                                type: 'input',
                                requirements: value => {return value !== undefined && value !== ''},
                                requirementDescription: this.dict.translate('$MayNotBeEmpty', 'dialog')
                            }
                        ]
                    }}
                ></InputBox>
                <MessageBox boxStyle="YesNo" dict={this.dict} visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onYesNoBoxClose(wb);}}/>
                <MessageBox boxStyle="Ok" dict={this.dict} visible={this.state.okBoxVisible} message={this.state.okMessage} onButtonPress={(wb)=>{this.onOkBoxClose(wb);}}/>
            </div>
        )
    }
}

export default connect(state => {
    return {}
})(DebuggerSettings);