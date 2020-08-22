import React from 'react';

// Components
import Rodal from 'rodal';
import StyledButton from './button';
import {FlatForm} from './forms';

// Styles
import './styles/input_box.css';

/**
 * Props:
 *  - visible: is visible?
 *  - message: what to display
 *  - onButtonPress: fired when a button is pressed
 *  - schema: A schema for the input box
 *  - dict: Translation dictionary
 */
class InputBox extends React.Component
{
    state = {
        rodalVisible: false,
        rodalMessage: ''
    }

    constructor(props)
    {
        super(props)

        if (this.props.dict === undefined)
            this.dict = {translate: (x) => {return x;}}
        else
            this.dict = this.props.dict;

        if (this.props.boxStyle === undefined)
            this.boxStyle = "Ok";
        else
            this.boxStyle = this.props.boxStyle
    }

    onRodalClose()
    {
        this.setState({
            rodalVisible: false
        })
    }

    showRodal(message)
    {
        this.setState({
            rodalMessage: message,
            rodalVisible: true
        })
    }

    onButtonClick(whatButton)
    {
        this.flatForm.resetAnimation();
        if (whatButton === "Ok" && !this.flatForm.allRequirementsSatisfied())
            return;
        this.props.onButtonPress(whatButton, this.flatForm.getInput());
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
            <Rodal 
                closeOnEsc={true} 
                showCloseButton={false} 
                visible={this.props.visible} 
                onClose={()=>{this.onRodalClose()}}
                enterAnimation={"fade"}
                leaveAnimation={"fade"}
            >
                <FlatForm 
                    ref={this.setFormRef}
                    dict={this.props.dict}
                    schema={this.props.schema}
                    environments={this.props.environments}
                ></FlatForm>
                <div className="inputBoxButtons">
                    <StyledButton className={"dialogButton"} onClick={()=>{this.onButtonClick("Ok")}}>{this.dict.translate("$Ok", 'dialog')}</StyledButton>
                    <StyledButton className={"dialogButton"} onClick={()=>{this.onButtonClick("Cancel")}}>{this.dict.translate("$Cancel", 'dialog')}</StyledButton>
                </div>
            </Rodal>
        );
    }
}

export default InputBox;