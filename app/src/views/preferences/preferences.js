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
        settings: {},
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
            if (_.isEqual(this.state.settings, this.origSettings))
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
            this.flatForm.getValues().map(item => {
                json.preferences[item.key] = (() => {
                    if (item.value === undefined)
                    {
                        switch(item.type)
                        {
                            case "input": return '';
                            case "boolbox": return false;
                            default: return undefined;
                        }
                    }
                    else
                        return item.value
                })();
            })
        }
        
        if (notAsString)
            return json;
        return JSON.stringify(json, null, 4);
    }

    /**
     * Transform actual representation to enforced representation by table.
     */
    fromJson = (json, initial) => 
    {
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
        
    }

    cancel = () => 
    {
        ipcRenderer.sendSync('closePrefWindow', '');
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
                    <FlatForm
                        className={"preferencesForm"}
                        ref={this.setFormRef}
                        dict={this.props.dict}
                        schema={this.schema}
                    >
                    </FlatForm>
                </JsonOptions>
                <MessageBox boxStyle="YesNo" dict={this.dict} visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onYesNoBoxClose(wb);}}/>
                <MessageBox boxStyle="Ok" dict={this.dict} visible={this.state.okBoxVisible} message={this.state.okMessage} onButtonPress={(wb)=>{this.onOkBoxClose(wb);}}/>
            </div>
        );
    }
}

export default Preferences;