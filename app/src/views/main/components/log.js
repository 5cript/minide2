import React from 'react';

import classNames from 'classnames';

import {Terminal} from 'xterm/lib/xterm';
import {FitAddon } from 'xterm-addon-fit';

// style
import './styles/log.css';
import '../../../elements/styles/various_inputs.css';
import './styles/xterm.css';

function uuid() 
{
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) 
    {
        var r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
        return v.toString(16);
    });
}

class LogPanel extends React.Component 
{
    constructor(props)
    {
        super(props)
        this.terminalId = uuid();
        this.lastText = '';

        const style = getComputedStyle(document.body);
        this.theme = {
            background: style.getPropertyValue('--background-color-darker'),
            color: style.getPropertyValue('--foreground-color'),
        }
    }

    componentDidMount = () =>
    {
        this.mounted = true;
    }

    componentDidUpdate = () => 
    {
        setTimeout(() => {
            this.init();
        }, 200);

        if (this.term === undefined || this.term === null)
            return;

        this.textArea.scrollTop = this.textArea.scrollHeight        
        let copyOver = this.lastText.length < this.textArea.value.length;
        
        if (
            this.lastText.length > 0 && 
            (this.lastText[0] !== this.textArea.value[0] || this.lastText.length > this.textArea.value.length)
        )
        {
            copyOver = true;
            this.lastText = '';
            this.term.clear();
        }
        if (copyOver)
        {
            this.write(this.textArea.value.substr(this.lastText.length, this.textArea.value.length - this.lastText.length))
            this.lastText = this.textArea.value;
        }
        //this.refit();
    }

    init = () =>
    {
        if (this.mounted !== true || this.term !== undefined)
            return;

        this.term = new Terminal({
            fontSize: 12,
            theme: {
                background: this.theme.background,
                color: this.theme.color
            },
            cols: 200
        });
        this.hostDiv = document.getElementById(this.terminalId);
        this.term.open(this.hostDiv);
        
        this.fitAddon = new FitAddon();
        this.term.loadAddon(this.fitAddon);

        this.hostDiv.addEventListener('mouseup', e => 
        {
            this.onMouseClick(e.offsetX, e.offsetY)
        })
        this.hostDiv.addEventListener('dblclick', e => 
        {
            this.onDoubleClick()
        })
        const thisDeleg = this
        this.term._core._viewportElement.addEventListener('scroll', function (e) 
        {
            const viewport = thisDeleg.term._core._viewportElement;
            const top = viewport.scrollTop
            const totalRows = top / thisDeleg.term._core._renderService.dimensions.actualCellHeight;
            const offset = totalRows;
            thisDeleg.scrollOffset = offset;
        });
    }

    onMouseClick = (viewX, viewY) => 
    {
        if (this.term === undefined || this.term === null)
            return;

        const cellWidth = this.term._core._renderService.dimensions.actualCellWidth;
        const cellHeight = this.term._core._renderService.dimensions.actualCellHeight;
        if (cellHeight === 0 || cellWidth === 0)
            return;
        const x = Math.floor(viewX / cellWidth);
        const y = Math.floor(viewY / cellHeight);
        this.onLineClick(x, Math.floor(y + this.scrollOffset));
    }

    onLineClick = (x, line) => 
    {
        if (this.term === undefined || this.term === null)
            return;

        if (!isFinite(line) || line < 0)
            return;

        this.term.selectLines(line, line);
        this.clickedLine = line;
        this.clickedX = x;
    }

    onDoubleClick()
    {
        if (this.term === undefined || this.term === null)
            return;

        if (this.props.onDoubleClick)
            this.props.onDoubleClick(this.clickedX, this.clickedLine);
    }

    getLine(y)
    {
        const split = this.textArea.value.split(/(?:\r\n|\r|\n)/g);
        if (y >= split.length || y < 0)
            return "";
        return split[y];
    }

    getSelection()
    {
        if (this.term)
            return this.term.getSelection();
        return "";
    }

    refit()
    {
        if (this.props.isVisible && this.fitAddon)
            this.fitAddon.fit();
    }

    setTextAreaRef = (node) => 
    {
        this.textArea = node;
    }

    write(text)
    {
        if (this.term === undefined || this.term === null)
            return;

        const append = text.replace(/(?:\r\n|\r|\n)/g, '\r\n');
        this.term.write(append);
    }

    render()
    {
        return (
            <div className='logPanelContainer'>
                <div 
                    className='logPanelHead'>
                </div>
                <div 
                    className={classNames('logPanelContent')}
                    id={this.terminalId}
                >
                </div>
                <textarea  
                    ref={this.setTextAreaRef}
                    type="text"
                    readOnly 
                    hidden
                    value={this.props.data}
                    className={classNames("logTextField", "minimalInput")}
                ></textarea>
            </div>
        )
    }
}

export default LogPanel