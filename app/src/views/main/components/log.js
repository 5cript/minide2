import React from 'react';

import classNames from 'classnames';

import {Terminal} from 'xterm/lib/xterm';
import {FitAddon} from 'xterm-addon-fit';
import _ from 'lodash';

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

        this.slowRefit = _.debounce(() => {
            if (this.term === undefined)
                console.error('terminal is uninitialized')
            else
                this.refit();
        }, 300);
    }

    componentDidMount = () =>
    {
        this.mounted = true;
        if (this.delayer)
            clearTimeout(this.delayer);
        this.delayer = setTimeout(() => {
            this.init();
            this.refit();
        }, 250);
    }

    transferText = () => 
    {
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
            const freshPart = this.textArea.value.substr(this.lastText.length, this.textArea.value.length - this.lastText.length);
            this.write(freshPart)
            this.lastText = this.textArea.value;
        }

    }

    componentDidUpdate = () => 
    {
        if (this.term)
            this.slowRefit();
        this.transferText();
    }

    init = () =>
    {
        console.log('tryinit');
        if (this.mounted !== true || this.term !== undefined)
            return;
        console.log('actuallyinit');

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

        this.transferText();
    }

    getRealLineFromView = (viewY) =>
    {
        if (this.term === undefined)
            return -1;

        let width = this.term.cols;
        const split = this.getTextLines();
        let virtualLinesPassed = 0;

        for (let i in split)
        {
            const cleansed = this.cleanLine(split[i]);
            let virtualLinesHere = Math.floor(cleansed.length / width);
            virtualLinesPassed += virtualLinesHere;
            if (viewY <= virtualLinesPassed)
                return _.toNumber(i);
            virtualLinesPassed += 1;
        }
        return -1;
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
        this.onLineClick(x, Math.round(y + (this.scrollOffset ? this.scrollOffset : 0)));
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

        const realY = this.getRealLineFromView(this.clickedLine);
        if (realY === -1)
            return;
        if (this.props.onDoubleClick)
        {
            this.props.onDoubleClick
            (
                this.clickedX, 
                realY,
                this.getRealLineText(realY)
            );
        }
    }

    getTextLines()
    {
        return this.textArea.value.split(/(?:\r\n|\r|\n)/g);
    }

    cleanLine(lineWithInstructions)
    {
        return lineWithInstructions.replace(/(?:\x9b|\x1b|\u001b)\[(?:(?:(?:[A-Z]|[0-9])(?:(?:[0-9][A-Z])|[0-9])?)(?:;(?:[0-9]){1,2})?(?:;(?:[0-9]){3})?)?[m|K]?/g, '');        
    }

    getRealLineText(realY)
    {
        const line = this.getTextLines()[realY];
        return this.cleanLine(line);
    }

    getLine(y)
    {
        const split = this.getTextLines();
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
        if (this.lastWidth === this.props.width && this.lastHeight === this.props.height)
            return;
        if (this.props.isVisible && this.fitAddon)
        {
            this.lastHeight = this.props.height;
            this.lastWidth = this.props.width;
            this.fitAddon.fit();
        }
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