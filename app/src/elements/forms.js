import React from 'react';

// Other
import classNames from 'classnames';
import _ from 'lodash';

// Components
import Tooltip from '@material-ui/core/Tooltip';
import Jello from 'react-reveal/Jello';
import WarningSharpIcon from '@material-ui/icons/WarningSharp';

// Styles
import './styles/forms.css';

let exampleDescription = 
{
    fields: [
        {
            key: 'firstName',
            label: 'First Name',
            type: 'input',
            properties: {},
            requirements: (value) => {return false;}
        },
    ]
}

export class FlatForm extends React.Component
{
    state = 
    {
        values: {},
        animates: {}
    }

    constructor(props)
    {
        super(props)

        if (this.props.key !== undefined)
            this.idPrefix = '__fform_' + this.props.key + '_';
        else
            this.idPrefix = '__fform_';
    }

    satisfiesRequirements = (scheme) =>
    {
        return !(scheme.requirements && scheme.requirements(this.state.values[this.qualifyId(scheme.key)]) !== true);
    }

    inputField = (scheme) => 
    {
        return (
            <div className="formInputField">
                <div className="formLabel">{scheme.label}</div>
                <Jello key={this.qualifyId(scheme.key) + 'j'} when={this.state.animates[this.qualifyId(scheme.key)] === true}>
                    <div className="formInputGroup">
                        <input 
                            {...scheme.properties} 
                            id={this.idPrefix + scheme.key} 
                            onChange={e => this.setValue(e.target.id, e.target.value)}
                            value={this.state.values[this.qualifyId(scheme.key)] === undefined ? '' : this.state.values[this.qualifyId(scheme.key)]}
                            className={
                                classNames("formInput", (() => {
                                    if (!this.satisfiesRequirements(scheme))
                                        return 'formInputBadRequirements'
                                    else
                                        return ''
                                })())
                            }
                        ></input>
                        {(() => {
                            if (!this.satisfiesRequirements(scheme))
                                return (
                                    <Tooltip placement="top" title={scheme.requirementDescription}>
                                        <WarningSharpIcon></WarningSharpIcon>
                                    </Tooltip>
                                );
                            else
                                return <div/>
                        })()}
                    </div>
                </Jello>
            </div>
        );
    }

    setValue = (id, value) => 
    {
        let vals = _.cloneDeep(this.state.values);
        vals[id] = value;

        this.setState({
            values: vals
        })
    }

    unknownType = (scheme) => 
    {
        return <div>{this.props.dict.translate("$UnknownSchemeType", "dialog") + ": " + scheme.label}</div>
    }

    qualifyId = (id) => 
    {
        return this.idPrefix + id;
    }

    getInput = () => 
    {
        const r = this.props.schema.fields.map(scheme => {
            return {
                ...scheme,
                value: this.state.values[this.qualifyId(scheme.key)]
            }
        });
        return r;
    }

    allRequirementsSatisfied = () =>
    {
        let ani = {};
        let anyUnsatisfied = false;
        for (const scheme of this.props.schema.fields)
        {
            if (!this.satisfiesRequirements(scheme))
            {
                anyUnsatisfied = true;
                ani[this.qualifyId(scheme.key)] = true;
            }
        }
        this.setState({
            animates: ani
        })
        return !anyUnsatisfied;
    }

    resetAnimation = () => 
    {
        this.setState({
            animates: {}
        })
    }

    reset = () => 
    {
        this.setState({
            values: {}
        })
    }

    render()
    {
        return (
            <div>
                {this.props.schema.fields.map(scheme => {
                    switch(scheme.type)
                    {
                        case('input'): return this.inputField(scheme);
                        default: return this.unknownType(scheme);
                    }
                })}
            </div>
        )
    }
}