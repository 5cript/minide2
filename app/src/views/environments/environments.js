import React from 'react';

// Components
import JsonOptions from '../../elements/json_options';
import { Combobox } from 'react-widgets';
import StyledButton from '../../elements/button';
import EditableList from '../../elements/editable_list';
import EditableTable from '../../elements/editable_table';
import MessageBox from '../../elements/message_box';
import InputBox from '../../elements/input_box';

// Other
import Dictionary from '../../util/localization';
import _ from 'lodash';
import classNames from 'classnames';
//import "../../util/ace_sane_theme";
// https://github.com/securingsincity/react-ace/issues/126#issuecomment-345151567

// Styles
import './styles/environments.css';
import '../../styles/react-widgets.css';

// Requires

class Environments extends React.Component 
{
    dict = new Dictionary();

    state = 
    {
        selectedEnvironment: '',
        environments:
        {
            dummy: 
            {
                path: [
                    'D:/msys2/mingw64/bin',
                    'D:/msys2/mingw64/bin2',
                    'D:/msys2/mingw64/bin3',
                    'D:/msys2/mingw64/bin4'
                ],
                variables: [ // format is not that of server, must be transformed
                    {key: 'A', value: 'B'},
                    {key: 'C', value: 'D'}
                ],
                inherits: {0: ''}
            }
        },
        yesNoBoxVisible: false,
        yesNoMessage: 'You should never see this',
        okBoxVisible: false,
        okMessage: 'You should never see this',
        profileInputVisible: false
    }


    environmentList = () =>
    {
        let res = [];
        for (let env in this.state.environments)
            res.push(env);
        return res;
    }

    selectedPath = () =>
    {
        if (this.validProfileSelected())
            return this.state.environments[this.state.selectedEnvironment].path;
        else
            return [];
    }
    
    onChangePathVar = (i, value) => 
    {
        if (!this.validProfileSelected())
            return;

        let envCopy = _.cloneDeep(this.state.environments);
        envCopy[this.state.selectedEnvironment].path[i] = value;
        this.setState({
            environments: envCopy
        })
    }

    onAddPath = (value) => 
    {
        if (!this.validProfileSelected())
            return;

        if (this.pathInput === undefined)
            return;

        let envCopy = _.cloneDeep(this.state.environments);
        envCopy[this.state.selectedEnvironment].path.push(this.pathInput.value);
        this.setState({
            environments: envCopy
        })
        this.pathInput.value = '';
    }

    setPathInputRef = (node) => 
    {
        this.pathInput = node;
    }

    onRemovePathItem = (index) => 
    {
        if (!this.validProfileSelected())
            return;

        if (index < 0 || index >= this.selectedPath().length)
            return;

        let envCopy = _.cloneDeep(this.state.environments);
        envCopy[this.state.selectedEnvironment].path.splice(index, 1);
        this.setState({
            environments: envCopy
        });
    }

    getVariables = () => 
    {
        if (!this.validProfileSelected())
            return [];
        return this.state.environments[this.state.selectedEnvironment].variables;
    }

    modifyVariable = (rowIndex, columnId, value) => 
    {
        if (!this.validProfileSelected())
            return;

        let envCopy = _.cloneDeep(this.state.environments);
        let v = envCopy[this.state.selectedEnvironment].variables[rowIndex];
        if (columnId === 'key')
            v.key = value;
        else
            v.value = value;
        this.setState({
            environments: envCopy
        });
    }

    deleteVariable = (rowIndex) => 
    {
        if (!this.validProfileSelected())
            return;

        let envCopy = _.cloneDeep(this.state.environments);
        envCopy[this.state.selectedEnvironment].variables.splice(rowIndex, 1);
        this.setState({
            environments: envCopy
        });
    }

    addLine = () => 
    {
        if (!this.validProfileSelected())
            return;

        let envCopy = _.cloneDeep(this.state.environments);
        envCopy[this.state.selectedEnvironment].variables.push({key: '', value: ''})
        this.setState({
            environments: envCopy
        });
    }

    validProfileSelected = () => 
    {
        let selectionIsNotEmpty = (this.state.selectedEnvironment !== undefined && this.state.selectedEnvironment !== '' && this.state.selectedEnvironment !== null);
        if (!selectionIsNotEmpty)
            return false;

        return this.state.environments.hasOwnProperty(this.state.selectedEnvironment);
    }

    onProfileSelect = (profile) => 
    {
        this.setState({
            selectedEnvironment: profile
        });
    }

    addEnvironment = () => 
    {
        this.setState({profileInputVisible: true});
    }

    deleteEnvironment = () => 
    {
        let reallyDelete = () => 
        {
            let envCopy = _.cloneDeep(this.state.environments);
            delete envCopy[this.state.selectedEnvironment];
            this.setState({
                environments: envCopy,
                selectedEnvironment: undefined
            });            
        };

        if (this.validProfileSelected())
            this.showYesNoBox(this.dict.translate('$ConfirmProfileRemove', 'environments'), reallyDelete)
    }

    uiComponents()
    {
        return (
            <div className="envSettingsContainer">
                <div className="pickerGroup">
                    <Combobox className="envPicker" data={this.environmentList()} value={this.state.selectedEnvironment} onChange={(...args) => {this.onProfileSelect(...args)}}></Combobox>
                    <StyledButton onClick={() => {this.addEnvironment()}}>{this.dict.translate("$Add", 'environments')}</StyledButton>
                    <StyledButton onClick={() => {this.deleteEnvironment()}}>{this.dict.translate("$Delete", 'environments')}</StyledButton>
                </div>
                <div className="lineSep"></div>
                <div className="singleEnvironment">
                    <div id="pathLabel">{this.dict.translate("$PathLabel", 'environments')}</div>
                    <EditableList 
                        enabled={this.validProfileSelected()}
                        className="pathBox" 
                        items={this.selectedPath()} 
                        onChange={
                            (index, value) => {this.onChangePathVar(index, value);}
                        }
                        onRemove={
                            (index) => {this.onRemovePathItem(index);}
                        }
                    ></EditableList>
                    <div id="addPathLineSection">
                        <input 
                            disabled={!this.validProfileSelected()} 
                            ref={this.setPathInputRef} 
                            className={classNames("styledInput", "pathInput", !this.validProfileSelected() ? 'disabledPathInput' : '')}
                            onKeyPress={e => {
                                if (e.key === "Enter") this.onAddPath();
                            }}
                        >
                        </input>
                        <StyledButton onClick={this.onAddPath}>
                            {this.dict.translate("$Add", 'environments')}
                        </StyledButton>
                    </div>
                    <div id="variablesLabel">{this.dict.translate("$VariablesLabel", 'environments')}</div>
                    <EditableTable 
                        className={classNames("envVariables", !this.validProfileSelected() ? 'disabledEnvVariables' : '')}
                        columns={[
                            {
                                Header: this.dict.translate('$Key', 'environments'),
                                accessor: 'key',
                                width: 200
                            },
                            {
                                Header: this.dict.translate('$Value', 'environments'),
                                accessor: 'value',
                                width: 300
                            }
                        ]}
                        onChange={this.modifyVariable}
                        onDelete={this.deleteVariable}
                        addLine={this.addLine}
                        data={this.getVariables()}
                    >
                    </EditableTable>
                </div>
            </div>
        );
    }

    /**
     * Transform forced representation by table view to actual representation
     */
    toJson = () => 
    {
        let envs = _.cloneDeep(this.state.environments);
        for (let envKey in envs)
        {
            let env = envs[envKey];
            let vars = {};
            for (let i in env.variables)
            {
                let v = env.variables[i];
                if (v.key === undefined || v.key === '' || v.key === null)
                    continue;

                vars[v.key] = v.value
            }
            delete env.variables;
            env.variables = vars;
        }
        return JSON.stringify(envs, null, 4);
    }

    /**
     * Transform actual representation to enforced representation by table.
     */
    fromJson = (jsonString) => 
    {
        if (jsonString === undefined)
            return;

        let envs = JSON.parse(jsonString);

        for (let envKey in envs)
        {
            let env = envs[envKey];
            let vars = [];
            for (let k in env.variables)
            {
                vars.push({
                    key: k,
                    value: env.variables[k]
                });
            }
            delete env.variables;
            env.variables = vars;
        }
        this.setState({
            environments: envs
        });

    }

    showYesNoBox(message, yesAction) 
    {
        this.setState({
            yesNoBoxVisible: true,
            yesNoMessage: message
        })
        this.yesAction = yesAction;
    }

    onYesNoBoxClose(whatButton)
    {
        this.setState({
            yesNoBoxVisible: false
        });
        if (whatButton === "Yes")
            this.yesAction();
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
        if (whatButton === "Yes")
            this.yesAction();
    }

    onProfileFormButtonPress(whatButton, input)
    {
        this.setState({profileInputVisible: false});

        if (whatButton === "Cancel")
            return;

        const i = input.find(e => e.key === 'profile')
        const profileName = i.value;

        if (this.state.environments.hasOwnProperty(profileName))
        {
            this.showOkBox(this.dict.translate('$ProfileAlreadyExists', 'environments'));
            return;
        }

        let envCopy = _.cloneDeep(this.state.environments);
        envCopy[profileName] = {
            path: [],
            variables: [],
            inherits: []
        };

        this.setState({
            environments: envCopy,
            selectedEnvironment: profileName
        })

        this.inputBox.reset();
    }

    setInputBoxRef = (node) => 
    {
        this.inputBox = node;
    }

    render()
    {
        return (
            <div>
                <JsonOptions 
                    dict={this.dict} 
                    json={this.toJson()}
                    onJsonUpdate={j => this.fromJson(j)}
                >
                    {this.uiComponents()}
                </JsonOptions>
                <MessageBox boxStyle="YesNo" dict={this.dict} visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onYesNoBoxClose(wb);}}/>
                <MessageBox boxStyle="Ok" dict={this.dict} visible={this.state.okBoxVisible} message={this.state.okMessage} onButtonPress={(wb)=>{this.onOkBoxClose(wb);}}/>
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
                                label: this.dict.translate('$ProfileName', 'environments'),
                                type: 'input',
                                requirements: value => {return value !== undefined && value !== ''},
                                requirementDescription: this.dict.translate('$MayNotBeEmpty', 'dialog')
                            }
                        ]
                    }}
                ></InputBox>
            </div>
        );
    }
};

export default Environments;