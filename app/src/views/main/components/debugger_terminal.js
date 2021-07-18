import React from 'react';
import {connect} from 'react-redux';

// Components
//import '../../../elements/styles/vari'

// Utility
import classNames from 'classnames';
import {splitCommandLine} from '../../../util/command_line_splitter';

// Styles
import './styles/debugger_terminal.css';

class DebuggerTerminal extends React.Component
{
    componentDidUpdate = () => 
    {
        if (this.textArea)
            this.textArea.scrollTop = this.textArea.scrollHeight;
    }

    onSubmit = (value) => 
    {
        /*
        const split = splitCommandLine(value);

        this.props.debugController.sendCommand({
            instanceId: this.props.instanceId,
            command: split.command,
            options: split.arguments
        });
        */

        this.props.debugController.sendRawCommand({
            instanceId: this.props.instanceId,
            command: value
        });
    }

    render = () =>
    {
        let consoleData = this.props.debugging.instances[this.props.instanceId].consoleStream;
        if (consoleData === undefined || consoleData === null)
            consoleData = "";

        const alive = this.props.debugging.instances[this.props.instanceId].debuggerAlive;

        return (
            <div className="debuggerTerminal">
                <div className="debuggerTerminalStatusBar">
                    <div>{this.props.dict.translate('$InstanceId', 'debugger_terminal') + ": "}</div>
                    <div className="debuggerTerminalInstanceId" style={{
                        color: alive ? "rgb(219, 255, 153)" : "rgb(255, 128, 128)"
                    }}>{this.props.instanceId}</div>
                    <div
                        style={{display: alive ? 'none' : null}}
                    >
                        {"(" + this.props.dict.translate('$DebuggerDied', 'debugger_terminal') + ")"}
                    </div>
                </div>
                <textarea 
                    ref={tarea => this.textArea = tarea}
                    className="debuggerOutput" 
                    readOnly 
                    onFocus={() => {if (this.input) this.input.focus();}}
                    value={consoleData}
                />
                <div className="debuggerInputArea">
                    <div className="debuggerTerminalPS1">{">"}</div>
                    <input 
                        ref={input => this.input = input} 
                        className={classNames("minimalInput", "debuggerTerminalInput")}
                        onKeyUp={event => {
                            if (event.keyCode === 13)
                            {
                                this.onSubmit(this.input.value);
                                this.input.value = "";
                            }
                        }}
                    >
                    </input>
                </div>
            </div>
        );
    }
};

export default connect(state => {
    return {
        logs: state.logs,
        activeLog: state.logs.activeLog,
        debugging: state.debugging
    }
}, null, null, {forwardRef: true})(DebuggerTerminal);