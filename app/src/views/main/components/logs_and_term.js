import React from 'react';
import {connect} from 'react-redux';

// Components
/*import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';*/
import Tabs from '@material-ui/core/Tabs';
import Tab from '@material-ui/core/Tab';

import TerminalInstance from './terminal';
import LogPanel from './log';

// Actions
import {setActiveLog} from '../../../actions/log_actions.js';

// Other
import _ from 'lodash'
import { withStyles } from '@material-ui/core/styles';

// Styles
import './styles/logs_and_term.css';
import './styles/tabs.css';

const LeanTabs = withStyles({
    root: {
        height: '25px',
        minHeight: 0,
        backgroundColor: 'var(--background-color-darker)'
    },
    indicator: {
        backgroundColor: 'var(--theme-color-brighter)',
    },
})(Tabs);

const LeanTab = withStyles((theme) => ({
    root: {
        textTransform: 'none',
        minWidth: 50,
        height: 25,
        paddingTop: 0,
        minHeight: 0,
        fontFamily: [
            '-apple-system',
            'BlinkMacSystemFont',
            '"Segoe UI"',
            'Roboto',
            '"Helvetica Neue"',
            'Arial',
            'sans-serif',
            '"Apple Color Emoji"',
            '"Segoe UI Emoji"',
            '"Segoe UI Symbol"',
        ].join(','),
        '&:hover': {
            opacity: 0.8,
        },
        '&$selected': {
            color: 'var(--theme-color)',
            fontWeight: theme.typography.fontWeightMedium,
            backgroundColor: 'var(--background-color-very-dark)'
        },
        '&:focus': {
            color: 'var(--theme-color)',
            backgroundColor: 'var(--background-color-very-dark)'
        },
    },
    selected: {},
}))((props) => <Tab disableRipple {...props} />);

function TabPanel(props)
{
    const { children, value, index, ...other } = props;
    return (
        <div
            role="tabpanel"
            hidden={value !== index}
            id={`simple-tabpanel-${index}`}
            aria-labelledby={`simple-tab-${index}`}
            style={{widht: "100%", height: "100%"}}
        >
            {children}
        </div>
    )
}

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

    refit = () => 
    {
        if (this.term)
            this.term.refit();
        for (const panel in this.panels)
        {
            this.panels[panel].refit();
        }
    }
    
    render = () => {
        return (
            <div className='tabContainer'>
                <LeanTabs 
                    value={this.props.activeLog}
                    onChange={(event, tabIndex) => {this.props.dispatch(setActiveLog(tabIndex))}}
                    className={'ReactTabs'}
                >
                    <LeanTab key="__terminal" label="Terminal"></LeanTab>
                    {(() => {
                        let res = []
                        for (const [key, ] of Object.entries(this.props.logs)) 
                        {
                            if (key !== "otherLogState")
                                res.push(
                                    <LeanTab key={key} label={key}></LeanTab>
                                )
                        }
                        return res
                    })()}
                </LeanTabs>
                <div className={'panelContainer'}>
                    <TabPanel 
                        key="__terminalPanel" 
                        className='terminalPanel'
                        value={this.props.activeLog}
                        index={0}
                        isVisible={0 === this.props.activeLog}
                    >
                        <TerminalInstance 
                            ref={this.setTermRef} 
                            height={this.props.height}
                        ></TerminalInstance>
                    </TabPanel>
                    {
                        Object.entries(this.props.logs).filter(e => e[0] !== "otherLogState").map((elem, i) => 
                        {
                            return <TabPanel 
                                key={elem[0]} 
                                index={i+1}
                                value={this.props.activeLog}
                            >
                                <LogPanel 
                                    ref={panel => this.setLogPanel(panel, elem[0], i)} 
                                    data={elem[1].data}
                                    className='logPanel'
                                    onDoubleClick={(x, y, yText) => {
                                        if (elem[1].type === 1)
                                        {
                                            const panel = this.panels[elem[0]];
                                            if (panel)
                                            {
                                                this.props.backend.toolbar().logDoubleClick(this.props.activeToolbar, elem[0], Number(y), yText);
                                            }
                                        }
                                    }}
                                    isVisible={i + 1 === this.props.activeLog}
                                >
                                </LogPanel>
                            </TabPanel>
                        })
                    }
                </div>
            </div>
        );
    }
}

export default connect(state => {
    return {
        logs: state.logs,
        activeLog: state.logs.otherLogState.activeLog,
        activeToolbar: state.toolbars.activeToolbar
    }
}, null, null, {forwardRef: true})(LogsAndOthers);