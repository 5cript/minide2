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
import { DropdownList } from 'react-widgets'

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
            values: {},
            animates: {}
        }

        this.environments = 
            this.props.environments ? this.props.environments : ['error, please pass envs']

        if (this.props.onChange)
            this.onAnyChange = this.props.onChange;
        else
            this.onAnyChange = () => {};

        if (this.props.key !== undefined)
            this.idPrefix = '__fform_' + this.props.key + '_';
        else
            this.idPrefix = '__fform_';
    }

    satisfiesRequirements = (scheme) =>
    {
        return !(scheme.requirements && scheme.requirements(this.state.values[this.qualifyId(scheme.key)]) !== true);
    }

    inputField = (scheme, options) => 
    {
        return (
            <div className="formInputField" key={scheme.key}>
                <div className="formLabel">{scheme.label}</div>
                <Jello key={this.qualifyId(scheme.key) + 'j'} when={this.state.animates[this.qualifyId(scheme.key)] === true}>
                    <div className="formInputGroup">
                        <input 
                            {...scheme.properties} 
                            id={this.idPrefix + scheme.key} 
                            onChange={e => {
                                if (options && options.numberOnly)
                                {
                                    if (e.target.value === '' || /^[0-9\b]+$/.test(e.target.value))
                                        this.setValue(e.target.id, e.target.value);
                                }
                                else
                                    this.setValue(e.target.id, e.target.value)
                            }}
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
            <div className="formBoolbox" key={scheme.key}>
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

    spacer = (scheme) => 
    {
        return <div className="formSpacer"></div>
    }

    setValue = (id, value) => 
    {
        let vals = _.cloneDeep(this.state.values);
        vals[id] = value;

        this.onAnyChange(id, value);

        this.setState({
            values: vals
        })
    }

    unknownType = (scheme) => 
    {
        return <div key={scheme.key}>{this.props.dict.translate("$UnknownSchemeType", "dialog") + ": " + scheme.label}</div>
    }

    qualifyId = (id) => 
    {
        return this.idPrefix + id;
    }

    environment = (scheme) =>
    {
        return <div key={scheme.key} className="flatFormEnvironment">
            <div className="formLabel">{scheme.label}</div>
            <DropdownList
                id={this.qualifyId(scheme.key)}
                data={this.environments}
            ></DropdownList>
        </div>
    }

    getInput = () => 
    {
        if (this.props.schema.categories === undefined)
        {
            const r = this.props.schema.fields.map(scheme => {
                return {
                    ...scheme,
                    value: this.state.values[this.qualifyId(scheme.key)]
                }
            });
            return r;
        }
        else
        {
            let categorizedValues = {}
            for (const categoryIter in this.props.schema.categories) 
            {
                const category = this.props.schema.categories[categoryIter];
                categorizedValues[category.id] = {}
                for (const field of this.props.schema.fields)
                {
                    if (field.category === category.id)
                    {
                        categorizedValues[category.id][field.key] = {
                            ...field,
                            value: this.state.values[this.qualifyId(field.key)]
                        };
                    }
                }
            }
            return categorizedValues;
        }
    }

    getValues = () => 
    {
        return this.getInput();
    }

    setValues = (idValueList) => 
    {
        let values = _.clone(this.state.values);

        for (const id in idValueList)
        {
            values[this.qualifyId(id)] = idValueList[id];
        }
        this.setState({values: values});
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
        let reorganized = _.cloneDeep(this.props.schema.categories);

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
                    subcat = findAndInsert(subcat);
                }
            }
            return currentCategory
        }
        for (let cat of reorganized)
        {
            cat = findAndInsert(cat);
        }

        const renderCategory = (cat, i) =>
        {
            return (
                <div className="formCategoryOutward" key={i}>
                    <span className="formCategoryCaption">{cat.caption}</span>
                    <div 
                        className="formCategoryInward"
                        style={{
                            borderColor: cat.borderColor ? cat.borderColor : 'var(--foreground-color)'
                        }}
                    >
                    {
                        cat.fields.map(scheme => this.fieldToComponent(scheme))
                    }
                    </div>
                </div>
            )
        }

        return (
            <div className={this.props.className}>
            {
                reorganized.map((category, i) => renderCategory(category, i))
            }
            </div>
        )
    }

    fieldToComponent = (scheme) => 
    {
        switch(scheme.type)
        {
            case('input'): return this.inputField(scheme);
            case('boolbox'): return this.boolbox(scheme);
            case('numberInput'): return this.inputField(scheme, {numberOnly: true});
            case('spacer'): return this.spacer(scheme);
            case('environment'): return this.environment(scheme);
            default: return this.unknownType(scheme);
        }
    }

    renderWithoutCategories()
    {
        return (
            <div className={this.props.className}>
                {this.props.schema.fields.map(scheme => {
                    return this.fieldToComponent(scheme);
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