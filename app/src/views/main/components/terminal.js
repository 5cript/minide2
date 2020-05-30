import React from 'react';
//import { connect } from 'react-redux';
import Scrollbar from 'react-scrollbars-custom';

import Terminal from '../../../terminal-react/terminal';

// style
import './styles/terminal.css';

class TerminalInstance extends React.Component {
    state = {
        data: ''
    }

    onSubmit = (ps1, dat) => {
        this.setState({ data: this.state.data + ps1 + ' ' + dat + "\n" });
    }


    // <Terminal data={this.state.data} onSubmit={this.onSubmit}></Terminal>
    render = () => {
        return (
            <div className='terminalInstance'>
                <Scrollbar id="RSC" style={{ width: "100%", height: "100%" }}>
                    <Terminal data={this.state.data} onSubmit={this.onSubmit}></Terminal>
                </Scrollbar>
            </div>
        );
    }
}

export default TerminalInstance;