import React from 'react';

// Other
import classNames from 'classnames';
import _ from 'lodash';
import { withStyles } from '@material-ui/core/styles';

// Components
import Tooltip from '@material-ui/core/Tooltip';
import Jello from 'react-reveal/Jello';
import WarningSharpIcon from '@material-ui/icons/WarningSharp';
import Switch from '@material-ui/core/Switch';

// Styles
import './styles/forms.css';

const ThemedSwitch = withStyles({
    switchBase: {
        color: 'var(--foreground-color)',
        '&$checked': {
            color: 'var(--theme-color-extreme)',
        },
        '&$checked + $track': {
            backgroundColor: 'var(--theme-darker)',
        },
        '& + $track': {
            backgroundColor: 'var(--foreground-disabled)',
        },
    },
    checked: {},
    track: {},
})(Switch);

export class FlatForm extends React.Component
{
    constructor(props)
    {
        super(props)
        
        this.state = {
            values: this.props.initialValues ? this.props.initialValues : {},
            animates: {}
        }

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

    boolbox = (scheme) => 
    {
        return (
            <div className="formBoolbox">
                <ThemedSwitch
                    id={this.qualifyId(scheme.key)}
                    checked={this.state.values[this.qualifyId(scheme.key)] === undefined ? false : this.state.values[this.qualifyId(scheme.key)]}
                    onChange={e => {
                        this.setValue(e.target.id, e.target.checked);
                    }}
                ></ThemedSwitch>
                <div className="formBoolboxLabel">{scheme.label}</div>
            </div>
        )
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

    getValues = () => 
    {
        return this.getInput();
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

    renderWithCategories()
    {
        let reorganized = this.props.schema.categories;

        const findAndInsert = (currentCategory) => 
        {
            if (currentCategory.fields === undefined)
                currentCategory.fields = [];

            for (let field of this.props.schema.fields)
            {
                if (field.category === currentCategory.id)
                    currentCategory.fields.push(field);
            }

            if (currentCategory.categories !== undefined)
            {
                for (let subcat of currentCategory.categories)
                {
                    findAndInsert(subcat);
                }
            }
        }
        findAndInsert(reorganized);

        return (
            <div>
            {
                (() => {
                    return <div></div>
                })()
            }
            </div>
        )
    }

    renderWithoutCategories()
    {
        return (
            <div className={this.props.className}>
                {this.props.schema.fields.map(scheme => {
                    switch(scheme.type)
                    {
                        case('input'): return this.inputField(scheme);
                        case('boolbox'): return this.boolbox(scheme);
                        default: return this.unknownType(scheme);
                    }
                })}
            </div>
        )
    }

    render()
    {
        if (this.props.schema.categories !== undefined)
            return this.renderWithCategories();
        else
            return this.renderWithoutCategories();
    }
}