import React from 'react';

// Components
import MessageBox from '../../../elements/message_box';

// Actions
import { setActiveFile, removeOpenFile } from '../../../actions/open_file_actions';

// Others
import {connect} from 'react-redux';
import classnames from 'classnames';

// Style
import './styles/open_files.css';

class OpenFilesList extends React.Component
{
    state = {
        yesNoBoxVisible: false,
        yesNoMessage: 'blubber'
    }

    showYesNoBox(message) 
    {
        this.setState({
            yesNoBoxVisible: true,
            yesNoMessage: message
        })
    }

    onMessageBoxClose(whatButton)
    {
        this.setState({
            yesNoBoxVisible: false
        });
        if (whatButton === "Yes")
            this.yesAction();
    }

    render = () => {
        return (
            <div>
                <div id='OpenFilesList'>
                    {this.props.openFiles.map((file, i) => {return(
                        <div 
                            className={classnames({
                                'openFileItem': true,
                                'alternativeFileItem': i % 2 === 0,
                                'activeFileItem': this.props.activeFile === i
                            })}
                            key={file.path}
                            onClick={() => {
                                console.log(this.props);
                                this.props.dispatch(setActiveFile(i));
                            }}
                        >
                            <button 
                                className={classnames({
                                    'openFileXButton': true,
                                    'inactiveXButton': this.props.activeFile !== i
                                })}
                                id='closeCross' 
                                onClick={(e) => {
                                    if (!file.synchronized) 
                                    {
                                        this.showYesNoBox(this.props.dict.translate("$CloseUnsavedWarning", "dialog"))
                                        this.yesAction = () => {
                                            console.log('yes action called');
                                            this.props.dispatch(removeOpenFile(file.path));     
                                        }
                                    }
                                    else
                                        this.props.dispatch(removeOpenFile(file.path)); 
                                    e.stopPropagation();
                                }}>
                            </button>
                            <div className='shortFileItemName'>
                                {
                                    (() => {
                                        if (!file.synchronized)
                                            return <svg viewBox="0 0 10 10" className="modifiedFile" xmlns="http://www.w3.org/2000/svg">
                                                <circle cx="5" cy="5" r="5" fill="red"/>
                                            </svg>
                                        return '';
                                    })()
                                }
                                {file.path}                            
                            </div>
                        </div>
                    )})}
                </div>
                <MessageBox boxStyle="YesNo" dict={this.props.dict} visible={this.state.yesNoBoxVisible} message={this.state.yesNoMessage} onButtonPress={(wb)=>{this.onMessageBoxClose(wb);}}/>
            </div>
        )
    }
}


export default connect(
    state => {
        return state.openFiles;
    }
)(OpenFilesList);