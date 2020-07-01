import React from 'react';
import {connect} from 'react-redux';

import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';

import {setActiveLog} from '../../../actions/log_actions.js';

import TerminalInstance from './terminal';
import LogPanel from './log';

import './styles/logs_and_term.css';
import './styles/tabs.css';

class LogsAndOthers extends React.Component
{
    state = {
        selectedTab: 0
    }

    setTermRef = node => 
    {
        this.props.onTermRef(node);
    }
    
    render = () => {
        return (
            <div className='tabContainer'>
                <Tabs 
                    selectedIndex={this.props.activeLog} 
                    onSelect={tabIndex => {this.props.dispatch(setActiveLog(tabIndex))}}
                    className={'ReactTabs'}
                >
                    <TabList>
                        <Tab key="__terminal">Terminal</Tab>
                        {(() => {
                            let res = []
                            for (const [key, ] of Object.entries(this.props.logs)) 
                            {
                                if (key !== "otherLogState")
                                    res.push(
                                        <Tab key={key}>{key}</Tab>
                                    )
                            }
                            return res
                        })()}
                    </TabList>
                    <TabPanel key="__terminalPanel" className='terminalPanel' style={{width: "100%", height: this.props.activeLog === 0 ? this.props.height: undefined}}>
                        <TerminalInstance ref={this.setTermRef} height={this.props.height}></TerminalInstance>
                    </TabPanel>
                    {(() => {
                        let res = []
                        for (const [key, value] of Object.entries(this.props.logs)) 
                        {
                            if (key !== "otherLogState")
                                res.push(
                                    <TabPanel key={key} style={{height: this.props.height}}>
                                        <LogPanel data={value.data}></LogPanel>
                                    </TabPanel>
                                )
                        }
                        return res
                    })()}
                </Tabs>
            </div>
        );
    }
}

export default connect(state => {
    return {
        logs: state.logs,
        activeLog: state.logs.otherLogState.activeLog
    }
})(LogsAndOthers);