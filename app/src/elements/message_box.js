import React from 'react';

// Components
import Rodal from 'rodal';
import StyledButton from './button';

// Styles
import './styles/message_box.css';

/**
 * Props:
 *  - visible: is visible?
 *  - message: what to display
 *  - onButtonPress: fired when a button is pressed
 *  - boxStyle: "YesNo", "Ok", "OkCancel"
 */
class MessageBox extends React.Component
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
        this.props.onButtonPress(whatButton);
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
                <div>{this.props.message}</div>
                {(()=>{
                    if (this.boxStyle === "YesNo") 
                    {
                        return (
                            <div className="buttonBox">
                                <StyledButton className={"dialogButton"} onClick={()=>{this.onButtonClick("Yes")}}>{this.dict.translate("$Yes", 'dialog')}</StyledButton>
                                <StyledButton className={"dialogButton"} onClick={()=>{this.onButtonClick("No")}}>{this.dict.translate("$No", 'dialog')}</StyledButton>
                            </div>
                        )
                    }
                    else if (this.boxStyle === "Ok") 
                    {
                        return (
                            <div className="buttonBox">
                                <StyledButton className={"dialogButton"} onClick={()=>{this.onButtonClick("Ok")}}>{this.dict.translate("$Ok", 'dialog')}</StyledButton>
                            </div>
                        )
                    }
                    else if (this.boxStyle === "OkCancel") 
                    {
                        return (
                            <div className="buttonBox">
                                <StyledButton className={"dialogButton"} onClick={()=>{this.onButtonClick("Ok")}}>{this.dict.translate("$Ok", 'dialog')}</StyledButton>
                                <StyledButton className={"dialogButton"} onClick={()=>{this.onButtonClick("Cancel")}}>{this.dict.translate("$Cancel", 'dialog')}</StyledButton>
                            </div>
                        )
                    }
                })()}
            </Rodal>
        );
    }
}

export default MessageBox;