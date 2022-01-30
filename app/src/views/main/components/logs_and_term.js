import React from 'react';
import {connect} from 'react-redux';

// Components
/*import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';*/

import TerminalInstance from './terminal';
import LogPanel from './log';
import DebuggerTerminal from './debugger_terminal';
import {TabPanel, MuiTabs} from '../../../elements/tabs';

// Actions
import {focusLogByName} from '../../../actions/log_actions';
import {debuggerSetFocussedInstance} from '../../../actions/debugging_actions';

// Other
//import _ from 'lodash'

// Styles
import './styles/logs_and_term.css';

class LogsAndOthers extends React.Component
{
    panels = {}

    setTermRef = (node) => 
    {
        this.term = node;
    }

    setLogPanel = (panel, key, index) =>
    {
        if (panel === null)
            return;
        this.panels[key] = panel;
        this.panels[key].index = index;
    }
    
    render = () => {
        let tabLabels = []
        let activeLabel = this.props.activeLog;
        for (let i in this.props.logs.ordering)
            tabLabels.push(this.props.logs.logs[this.props.logs.ordering[i]].logName)
        activeLabel = this.props.logs.ordering.findIndex(elem => elem === activeLabel);

        return (
            <div className='tabContainer'>
                <MuiTabs
                    onChange={(viewIndex) => {
                        const log = this.props.logs.logs.find(log => {
                            return log.logName === tabLabels[viewIndex];
                        })
                        if (log && log.logType === '_debug_terminal')
                        {
                            this.props.dispatch(debuggerSetFocussedInstance(log.instanceId));
                        }
                        this.props.dispatch(focusLogByName(tabLabels[viewIndex]))
                        
                    }}
                    value={activeLabel}
                    tabLabels={tabLabels}
                    id={this.props.tabsId}
                >
                    {
                        this.props.logs.logs.map((elem, i) => 
                        {
                            if (elem.logType === '_terminal')
                                return (
                                    <TabPanel 
                                        key={"__terminalPanel_" + elem.logName}
                                        className='terminalPanel'
                                        value={this.props.activeLog}
                                        index={i}
                                    >
                                        <TerminalInstance 
                                            ref={this.setTermRef} 
                                            height={this.props.height}
                                            width={this.props.width}
                                            isVisible={i === this.props.activeLog}
                                        ></TerminalInstance>
                                    </TabPanel>
                                )

                            else if (elem.logType === '_debug_terminal')
                            {
                                return (
                                    <TabPanel
                                        key={"_debug_terminal_" + elem.logName + elem.instanceId}
                                        className='debugTerminalPanel'
                                        value={this.props.activeLog}
                                        index={i}
                                    >
                                        <DebuggerTerminal
                                            instanceId={elem.instanceId}
                                            dict={this.props.dict}
                                            debugController={this.props.debugController}
                                        ></DebuggerTerminal>
                                    </TabPanel>
                                )
                            }

                            return (
                                <TabPanel 
                                    key={i} 
                                    index={i}
                                    value={this.props.activeLog}
                                >
                                    <LogPanel 
                                        ref={panel => this.setLogPanel(panel, elem.logName + i, i)} 
                                        data={elem.data}
                                        className='logPanel'
                                        height={this.props.height}
                                        width={this.props.width}
                                        isVisible={i === this.props.activeLog}
                                        onDoubleClick={(x, y, yText) => {
                                            if (elem.logType === 1)
                                            {
                                                const panel = this.panels[elem.logName + i];
                                                if (panel)
                                                {
                                                    this.props.backend.toolbar().logDoubleClick(this.props.activeToolbar, elem.logName, Number(y), yText);
                                                }
                                            }
                                        }}
                                    >
                                    </LogPanel>
                                </TabPanel>
                            );
                        })
                    }
                </MuiTabs>
            </div>
        );
    }
}

export default connect(state => {
    return {
        logs: state.logs,
        activeLog: state.logs.activeLog,
        activeToolbar: state.toolbars.activeToolbar
    }
}, null, null, {forwardRef: true})(LogsAndOthers);