import React from 'react';

// Components
import StyledButton from './button';
import ReactModal from 'react-modal-resizable-draggable';
import TextareaAutosize from 'react-textarea-autosize';

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
            <ReactModal initWidth={500} initHeight={200} 
                onFocus={() => {}}
                className={"messageBoxModal"}
                onRequestClose={()=>{this.onRodalClose()}} 
                isOpen={this.props.visible}
            >
                <div className="messageBoxHeader">{this.props.title}</div>
                <TextareaAutosize  
                    className="messageBoxText" 
                    value={this.props.message === undefined ? '' : this.props.message}
                    readOnly   
                    wrap='soft'
                >    
                </TextareaAutosize >
                <div className="messageBoxButtons">
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
                </div>
            </ReactModal>
        );
    }
}

export default MessageBox;