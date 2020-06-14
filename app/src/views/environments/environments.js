import React from 'react';

// Components
import JsonOptions from '../../elements/json_options';
import { Combobox } from 'react-widgets';
import StyledButton from '../../elements/button';
import EditableList from '../../elements/editable_list';
import EditableTable from '../../elements/editable_table';

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
        jsonRep: '{}',
        selectedEnvironment: 'dummy',
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
                variables: {'A': 'B'},
                inherits: {0: ''}
            }
        }
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
        return this.state.environments[this.state.selectedEnvironment].path;
    }
    
    onChangePathVar = (i, value) => 
    {
        let envCopy = _.cloneDeep(this.state.environments);
        envCopy[this.state.selectedEnvironment].path[i] = value;
        this.setState({
            environments: envCopy
        })
    }

    onAddPath = (value) => 
    {
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
        if (index < 0 || index >= this.selectedPath().length)
            return;

        let envCopy = _.cloneDeep(this.state.environments);
        envCopy[this.state.selectedEnvironment].path.splice(index, 1);
        this.setState({
            environments: envCopy
        });
    }

    uiComponents()
    {
        return (
            <div className="envSettingsContainer">
                <div className="pickerGroup">
                    <Combobox className="envPicker" data={this.environmentList()}></Combobox>
                    <StyledButton>{this.dict.translate("$Add", 'environments')}</StyledButton>
                    <StyledButton>{this.dict.translate("$Delete", 'environments')}</StyledButton>
                </div>
                <div className="lineSep"></div>
                <div className="singleEnvironment">
                    <div id="pathLabel">{this.dict.translate("$PathLabel", 'environments')}</div>
                    <EditableList 
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
                        <input ref={this.setPathInputRef} className={classNames("styledInput", "pathInput")}></input>
                        <StyledButton onClick={this.onAddPath}>{this.dict.translate("$Add", 'environments')}</StyledButton>
                    </div>
                    <div id="variablesLabel">{this.dict.translate("$VariablesLabel", 'environments')}</div>
                    <EditableTable className="variables"></EditableTable>
                </div>
            </div>
        );
    }

    render()
    {
        return (
            <JsonOptions 
                dict={this.dict} 
                json={JSON.stringify(this.state.environments, null, 4)}
                onJsonUpdate={j => {
                    this.setState({
                        environments: JSON.parse(j)
                    })
                }}
            >
                {this.uiComponents()}
            </JsonOptions>
        );
    }
};

export default Environments