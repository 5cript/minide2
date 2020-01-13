import React from 'react';

import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';

import './styles/logs_and_term.css';
import './styles/tabs.css';

class LogsAndOthers extends React.Component
{
    render = () => {
        return (
            <div>
                <Tabs>
                    <TabList>
                        <Tab>Terminal</Tab>
                        <Tab>Log</Tab>
                    </TabList>
                    <TabPanel>Terminal</TabPanel>
                    <TabPanel>Logs</TabPanel>
                </Tabs>
            </div>
        );
    }
}

export default LogsAndOthers;