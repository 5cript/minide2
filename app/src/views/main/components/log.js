import React from 'react';

import classNames from 'classnames';

// style
import './styles/log.css';
import '../../../elements/styles/various_inputs.css';

class LogPanel extends React.Component 
{
    setTextAreaRef = node => 
    {
        this.textArea = node;
    }

    componentDidUpdate = () =>
    {
        this.textArea.scrollTop = this.textArea.scrollHeight        
    }

    render()
    {
        return (
            <div className='logPanel'>
                <div className='logPanelHead'></div>
                <div className='logPanelContent'>
                    <textarea  
                        ref={this.setTextAreaRef}
                        type="text"
                        readOnly 
                        value={this.props.data}
                        className={classNames("logTextField", "minimalInput")}
                    ></textarea>
                </div>
            </div>
        )
    }
}

export default LogPanel