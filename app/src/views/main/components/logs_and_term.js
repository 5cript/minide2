import React from 'react';

import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';

import TerminalInstance from './terminal';

import './styles/logs_and_term.css';
import './styles/tabs.css';

class LogsAndOthers extends React.Component
{
    state = {
        selectedTab: 0
    }
    
    render = () => {
        return (
            <div className='tabContainer'>
                <Tabs selectedIndex={this.state.selectedTab} onSelect={tabIndex => {this.setState({selectedTab: tabIndex})}}>
                    <TabList>
                        <Tab >Terminal</Tab>
                        <Tab>Log</Tab>
                    </TabList>
                    <TabPanel className='terminalPanel' style={{width: "100%", height: "calc(100% - 25px)"}}>
                        <TerminalInstance></TerminalInstance>
                    </TabPanel>
                    <TabPanel>Logs</TabPanel>
                </Tabs>
            </div>
        );
    }
}

export default LogsAndOthers;