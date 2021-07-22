import React from 'react';

// Components
import JsonOptions from '../../elements/json_options';
import MessageBox from '../../elements/message_box';
import {FlatForm} from '../../elements/forms';

// Other
import Dictionary from '../../util/localization';
import _ from 'lodash';
import PreferencesSchema from './schema';

// Styles
import './styles/preferences.css';

// Requires
const {ipcRenderer} = window.require('electron');

class Preferences extends React.Component 
{
    dict = new Dictionary();

    state = 
    {
        preferences: {},
        yesNoBoxVisible: false,
        yesNoMessage: 'You should never see this',
        okBoxVisible: false,
        okMessage: 'You should never see this'
    }

    constructor(props)
    {
        super(props)

        this.origSettings = {};
        this.schema = PreferencesSchema(this.dict);

        ipcRenderer.on('closeIssued', (event, arg) => 
        {
            if (this.state.yesNoBoxVisible)
                this.cancel();

            // any unchanged files?
            if (_.isEqual(this.state.preferences, this.origSettings))
                this.cancel();
            else
            {
                this.showYesNoBox(this.dict.translate('$SaveChanges', 'environments'), () => {
                    this.save(() => {
                        this.cancel();
                    })
                }, () => {
                    this.cancel();
                })
            }
        });

        ipcRenderer.on('preferences', (event, arg) => {
            this.fromJson(arg);
            return null;
        })
    }

    /**
     * Transform forced representation by table view to actual representation
     */
    toJson = (notAsString) => 
    {
        let json = {}
        if (this.flatForm)
        {
            json.preferences = {};
            const categories = this.flatForm.getValues();
            for (let catIt in categories)
            {
                json.preferences[catIt] = {}
                for (let fieldIt in categories[catIt])
                {
                    const field = categories[catIt][fieldIt]
                    json.preferences[catIt][field.key] = (() => {
                        if (field.value === undefined)
                        {
                            switch(field.type)
                            {
                                case "input": return '';
                                case "boolbox": return false;
                                default: return undefined;
                            }
                        }
                        else
                            return field.value
                    })();
                }
            }
        }
        
        if (notAsString)
            return json;
        return JSON.stringify(json, null, 4);
    }
    
    fromJson = (jsonString) => 
    {
        const json = JSON.parse(jsonString)
        this.setState({settings: json});
        let idValueList = {}
        console.log(json)
        for (const catId in json.preferences)
        {
            for (const fieldId in json.preferences[catId])
            {
                idValueList[fieldId] = json.preferences[catId][fieldId]
            }
        }
        if (this.flatForm)
        {
            this.flatForm.setValues(idValueList);
        }
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

    save = (successAction) => 
    {        
        ipcRenderer.send('applicationPreferencesSaved', this.toJson(true));
        ipcRenderer.send('closePrefWindow', '');
        return '';
    }

    cancel = () => 
    {
        ipcRenderer.send('closePrefWindow', '');
    }

    load = () => 
    {
    }

    setFormRef = (node) => 
    {
        this.flatForm = node;
    }

    reset = () => 
    {
        this.flatForm.reset();
    }

    render()
    {
        return (
            <div>
                <JsonOptions 
                    dict={this.dict} 
                    json={this.toJson()}
                    provideJson={this.toJson}
                    onJsonUpdate={j => this.fromJson(j)}
                    onSave={() => {this.save()}}
                    onCancel={() => {this.cancel()}}
                >
                    <div className={"preferencesScrollArea"}>
                        <FlatForm
                            className={"preferencesForm"}
                            ref={this.setFormRef}
                            dict={this.props.dict}
                            schema={this.schema}
                            onChange={() => {
                                this.setState({settings: this.toJson()})
                            }}
                        >
                        </FlatForm>
                        <div style={{height: '20px'}}></div>
                    </div>
                </JsonOptions>
                <MessageBox boxStyle="YesNo" dict={this.dict} visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onYesNoBoxClose(wb);}}/>
                <MessageBox boxStyle="Ok" dict={this.dict} visible={this.state.okBoxVisible} message={this.state.okMessage} onButtonPress={(wb)=>{this.onOkBoxClose(wb);}}/>
            </div>
        );
    }
}

export default Preferences;