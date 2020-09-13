import React from 'react';
import {connect} from 'react-redux';

// Components
import DropdownList from 'react-widgets/lib/DropdownList';
import {FormGroup, FormControlLabel} from '@material-ui/core';
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
import ReduxPersistanceHelper from '../../util/redux_persist';

// Actions
import {setDebuggingProfiles, setGlobalDebuggerSettigns, setDebuggingProfile, setDebugInitialLoadDone} from '../../actions/debugging_actions';

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
        selectedProfile: '',
        profileInputVisible: false,
        yesNoBoxVisible: false,
        yesNoMessage: 'You should never see this',
        okBoxVisible: false,
        okMessage: 'You should never see this'
    }

    constructor(props)
    {
        super(props)

        this.saveDebounced = _.debounce(this.saveImmediately, 100, {maxWait: 500});
    }

    dndDragEnd = (dndOperation) =>
    {
    }

    saveImmediately = () =>
    {
        if (!this.props.debugging.initialLoadDone)
            return;
        const persistor = new ReduxPersistanceHelper(this.props.store);
        persistor.saveSelection('debugger_settings.json', state => {
            return {
                globalSettings: state.debugging.globalSettings,
                profiles: state.debugging.profiles
            }            
        });
    }
    
    save = () =>
    {
        this.saveDebounced();
    }

    load = () => 
    {
        const persistor = new ReduxPersistanceHelper(this.props.store);
        persistor.loadByDispatch('debugger_settings.json', [
            fileContent => setDebuggingProfiles(fileContent.profiles),
            fileContent => setGlobalDebuggerSettigns(fileContent.globalSettings),
            () => setDebugInitialLoadDone()
        ]);
        console.log('load');
    }

    getProfiles = () => 
    {
        return this.props.debugging.profiles.map(prof => prof.name);
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

        if (this.getProfiles().findIndex(elem => elem === profileName) !== -1)
        {
            this.showOkBox(this.dict.translate('$ProfileAlreadyExists', 'debugger_settings'));
            this.inputBox.reset();
            return;
        }

        const makeProfile = (name) => {
            return {
                name: name,
                debugger: 'gdb',
                path: 'gdb.exe',
                fullyReadSymbols: true,
                returnChildResult: true,
                initCommandFile: null,
                commandFile: null
            }
        };

        let profiles = _.cloneDeep(this.props.debugging.profiles);
        profiles.push(makeProfile(profileName));

        this.props.dispatch(setDebuggingProfiles(profiles));
        this.setState({
            selectedProfile: profileName
        })

        this.inputBox.reset();
    }

    deleteProfile = () => 
    {
        let reallyDelete = () => 
        {
            let profiles = _.cloneDeep(this.props.debugging.profiles);
            let profileIndex = profiles.findIndex(elem => elem.name === this.state.selectedProfile);
            if (profileIndex === -1)
                return;

            profiles.splice(profileIndex, 1);
            this.props.dispatch(setDebuggingProfiles(profiles));
            this.setState({
                selectedProfile: undefined
            });            
        };

        if (this.validProfileSelected())
            this.showYesNoBox(this.dict.translate('$ConfirmProfileRemove', 'debugger_settings'), reallyDelete)
    }

    setProfileProperty = (affected, value) => 
    {
        let o = {}
        o[affected] = value

        this.props.dispatch(setDebuggingProfile(this.state.selectedProfile, o, true))
    }

    onGlobalSwitchChange = (affected, value) => 
    {
        let o = _.clone(this.props.debugging.globalSettings);
        o[affected] = value;
        this.props.dispatch(setGlobalDebuggerSettigns(o));
    }

    componentDidUpdate = () =>
    {
        if (this.props.debugging.profiles.length > 0 && this.state.selectedProfile === '')
        {
            this.setState({
                selectedProfile: this.props.debugging.profiles[0].name
            })
        }
        this.save();
    }

    componentDidMount = () =>
    {
        this.load();
    }

    getProfileProperty = (property, defaul) =>
    {
        if (defaul === undefined)
            console.error('ERROR, getProfileProperty called with undefined default, this is not allowed');

        if (this.state.selectedProfile === undefined)
            return defaul;

        const profile = this.props.debugging.profiles.find(profile => {return profile.name === this.state.selectedProfile});
        if (profile === undefined)
            return defaul;

        const prop = profile[property];
        if (prop === undefined)
            return defaul;

        return prop;
    }

    debuggerPathComponent = () => 
    {
        return (
            <div
                className="debuggerPathInputFrame"
            >
                <label>{this.dict.translate("$Path", 'debugger_settings') + ":"}</label>
                <MinimalInput 
                    className="debuggerFormInput"
                    value={this.getProfileProperty('path', '')}
                    onChange={e => this.setProfileProperty('path', e.target.value)}
                />
            </div>
        );
    }

    additionalArgumentsComponents = () => 
    {
        return (
            <div className="debuggerAdditionalArgs">
                <label className="debuggerSwitchLabel">{this.dict.translate("$AdditionalCommandline", 'debugger_settings')}</label>
                <MinimalInput 
                    className="debuggerFormInput"
                    id="debuggerAdditionalArgBox"
                    type="text" 
                    value={this.getProfileProperty('additionCommandlineArguments', '')}
                    onChange={e => this.setProfileProperty('additionCommandlineArguments', e.target.value)}
                />
            </div>
        );
    }

    render() 
    {
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
                            <div className="debuggerAutoSaveBanner">
                                {this.dict.translate('$SettingsSaveImmediately', 'debugger_settings')}
                            </div>
                            <div id="generalSettings">
                                <FormGroup>
                                    <FormControlLabel
                                        control={<ThemedSwitch checked={this.props.debugging.globalSettings.buildBeforeRun} onChange={val => this.onGlobalSwitchChange('buildBeforeRun', val.target.checked)} name="buildBeforeDebug" />}
                                        label={this.dict.translate("$BuildBeforeDebug", 'debugger_settings')}
                                    />
                                    <FormControlLabel
                                        control={<ThemedSwitch checked={this.props.debugging.globalSettings.runBinaryImmediately} onChange={val => this.onGlobalSwitchChange('runBinaryImmediately', val.target.checked)} name="runBinary" />}
                                        label={this.dict.translate("$ImmediatelyRunBinary", 'debugger_settings')}
                                    />
                                    <FormControlLabel
                                        control={<ThemedSwitch checked={this.props.debugging.globalSettings.autoWatchLocals} onChange={val => this.onGlobalSwitchChange('autoWatchLocals', val.target.checked)} name="watchLocalVariables" />}
                                        label={this.dict.translate("$WatchLocalVariables", 'debugger_settings')}
                                    />
                                    <FormControlLabel
                                        control={<ThemedSwitch checked={this.props.debugging.globalSettings.autoWatchFunctionArguments} onChange={val => this.onGlobalSwitchChange('autoWatchFunctionArguments', val.target.checked)} name="watchFuncArgs" />}
                                        label={this.dict.translate("$WatchFunctionArguments", 'debugger_settings')}
                                    />
                                </FormGroup>
                            </div>
                        </TabPanel>
                        <TabPanel 
                            index={1}
                            value={this.state.activeTab}
                        >         
                            <div className="debuggerAutoSaveBanner">
                                {this.dict.translate('$SettingsSaveImmediately', 'debugger_settings')}
                            </div>               
                            <div id="debuggerSettingsProfileSelectBox">
                                <label id="debuggerSettingsProfileLabel">{this.dict.translate("$Profile", 'debugger_settings') + ":"}</label>
                                <DropdownList
                                    id="debuggerSettingsProfileDropdown"
                                    data={this.getProfiles()}
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
                                            onChange={value => {
                                                this.props.dispatch(setDebuggingProfile(this.state.selectedProfile, {
                                                    debugger: value
                                                }, true))
                                            }}
                                            value={this.getProfileProperty('debugger', '')}
                                        />
                                    </div>
                                </div>

                                {/*GDB*/}
                                <div id="gdbSettings" style={{display: this.getProfileProperty('debugger', '') !== 'gdb' ? 'none' : undefined}}>
                                    {this.debuggerPathComponent()}
                                    <div>
                                        <ThemedSwitch checked={this.getProfileProperty('fullyReadSymbols', false)} onChange={val => this.setProfileProperty('fullyReadSymbols', val.target.checked)} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$FullyReadSymbols", 'debugger_settings')}</label>
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.getProfileProperty('neverReadSymbols', false)} onChange={val => this.setProfileProperty('neverReadSymbols', val.target.checked)} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$NeverReadSymbols", 'debugger_settings')}</label>
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.getProfileProperty('returnChildResult', false)} onChange={val => this.setProfileProperty('returnChildResult', val.target.checked)} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$OutputChildResult", 'debugger_settings')}</label>
                                    </div>
                                    <div className="debuggerSettingsWithInput">
                                        <div>
                                            <ThemedSwitch checked={(() => {
                                                const initComFile = this.getProfileProperty('initCommandFile', false);
                                                return initComFile !== undefined && initComFile !== null;
                                            })()} onChange={val => {
                                                if (!val.target.checked)
                                                    this.setProfileProperty('initCommandFile', null)
                                                else
                                                    this.setProfileProperty('initCommandFile', '')
                                            }} />
                                            <label className="debuggerSwitchLabel">{this.dict.translate("$InitCommandFile", 'debugger_settings') + ":"}</label>
                                        </div>
                                        <MinimalInput 
                                            className={classNames("debuggerFormInput", "debuggerAdditionalInput", (() => {
                                                const initComFile = this.getProfileProperty('initCommandFile', false);
                                                return initComFile !== undefined && initComFile !== null;
                                            })() ? undefined : 'debuggerDisabledInput')} 
                                            type="text" 
                                            disabled={!(() => {
                                                const initComFile = this.getProfileProperty('initCommandFile', false);
                                                return initComFile !== undefined && initComFile !== null;
                                            })()} 
                                            value={(() => {
                                                const prop = this.getProfileProperty('initCommandFile', '');
                                                return prop === null ? '' : prop;
                                            })()}
                                            onChange={e => this.setProfileProperty('initCommandFile', e.target.value)}
                                        />
                                    </div>
                                    <div className="debuggerSettingsWithInput">
                                        <div>
                                            <ThemedSwitch checked={(() => {
                                                const comFile = this.getProfileProperty('commandFile', false);
                                                return comFile !== undefined && comFile !== null;
                                            })()} onChange={val => {
                                                if (!val.target.checked)
                                                    this.setProfileProperty('commandFile', null)
                                                else
                                                    this.setProfileProperty('commandFile', '')
                                            }} />
                                            <label className="debuggerSwitchLabel">{this.dict.translate("$CommandFile", 'debugger_settings') + ":"}</label>
                                        </div>
                                        <MinimalInput 
                                            className={classNames("debuggerFormInput", "debuggerAdditionalInput", (() => {
                                                const comFile = this.getProfileProperty('commandFile', false);
                                                return comFile !== undefined && comFile !== null;
                                            })() ? undefined : 'debuggerDisabledInput')} 
                                            type="text" 
                                            disabled={!(() => {
                                                const comFile = this.getProfileProperty('commandFile', false);
                                                return comFile !== undefined && comFile !== null;
                                            })()}  
                                            value={(() => {
                                                const prop = this.getProfileProperty('commandFile', '');
                                                return prop === null ? '' : prop;
                                            })()}
                                            onChange={e => this.setProfileProperty('commandFile', e.target.value)}
                                        />
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.getProfileProperty('ignoreGdbInit', false)} onChange={val => this.setProfileProperty('ignoreGdbInit', val.target.checked)} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$Ignore", 'debugger_settings') + " ~/.gdbinit"}</label>
                                    </div>
                                    <div>
                                        <ThemedSwitch checked={this.getProfileProperty('ignoreAllGdbInit', false)} onChange={val => this.setProfileProperty('ignoreAllGdbInit', val.target.checked)} />
                                        <label className="debuggerSwitchLabel">{this.dict.translate("$IgnoreAll", 'debugger_settings') + " .gdbinit"}</label>
                                    </div>
                                    {this.additionalArgumentsComponents()}
                                </div>

                                {/*LLDB*/}
                                <div id="lldbSettings" style={{display: this.getProfileProperty('debugger', '') !== 'lldb' ? 'none' : undefined}}>
                                    {this.debuggerPathComponent()}
                                    {this.additionalArgumentsComponents()}
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
                                autofocus: true,
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
    return {
        debugging: state.debugging,
        misc: state.misc
    }
})(DebuggerSettings);