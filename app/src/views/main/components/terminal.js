import React from 'react';
//import { connect } from 'react-redux';
import Scrollbar from 'react-scrollbars-custom';

import {Terminal} from 'xterm/lib/xterm';
import {FitAddon } from 'xterm-addon-fit';
import ResizeObserver from 'react-resize-observer';

// style
import './styles/terminal.css';
import 'xterm/css/xterm.css';

class TerminalInstance extends React.Component 
{
    constructor(props)
    {
        super(props);

        this.accumInput = '';
        this.path = '/'
        this.ps1 = '\x1B[1;3;32mName\x1B[0m $ ';
    }

    writePs1()
    {
        this.term.write('\x1B[1;3;31m' + this.path + '\x1B[0m ')
        this.term.write(this.ps1);
    }

    submit()
    {
        console.log('submit')
        this.term.write('\r\n');
        this.writePs1();
        this.accumInput = '';
    }

    handleInput(key, event)
    {
        if (key.charCodeAt(0) !== 13)
        {
            this.accumInput += key;
            this.term.write(key);
        }
        else
            this.submit();
    }

    initTerminal()
    {
        this.fitAddon = new FitAddon();
        this.term = new Terminal();
        this.term.loadAddon(this.fitAddon);
        const termContainer = document.getElementById('Terminal');
        this.term.open(termContainer);
        this.writePs1();
        
        // events:
        this.term.onKey((e) => {this.handleInput(e.key, e.domEvent);});
        this.term.onResize((e) => 
        {
            console.log(e);
        });
    }

    refit()
    {
        if (this.fitAddon)
            this.fitAddon.fit();
    }

    componentDidMount()
    {
        this.initTerminal();
    }

    // <Terminal data={this.state.data} onSubmit={this.onSubmit}></Terminal>
    render = () => {
        return (
            <div className='terminalInstance'>
                <div id="Terminal">
                    <ResizeObserver
                        onResize={(rect) => {
                            this.refit();
                        }}
                        onPosition={(rect) => {
                        }}
                    />
                </div>
            </div>
        );
    }
}

export default TerminalInstance;