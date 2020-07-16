import React from 'react';

// components
import Tabs from '@material-ui/core/Tabs';
import Tab from '@material-ui/core/Tab';
import {Droppable, Draggable} from 'react-beautiful-dnd';

// other
import { withStyles } from '@material-ui/core/styles';
import _ from 'lodash';
import generateId from '../util/random_id';

// styles
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

const getListStyle = isDraggingOver => ({
    background: isDraggingOver ? 'var(--background-color-brighter)' : undefined,
    display: 'flex',
    paddingTop: '2px',
    paddingBot: '2px',
    paddingLeft: '0px',
    overflow: 'auto',
});

const getItemStyle = (isDragging, draggableStyle) => ({
    // some basic styles to make the items look a bit nicer
    padding: '10px',
    fontSize: 10,
    // change background colour if dragging
    background: isDragging ? 'lightgreen' : undefined,
  
    // styles we need to apply on draggables
    ...draggableStyle,
  });
  

class SleekTabs extends React.Component
{
    /*
    * props:
    *   id: required for drop identification,
    *   value: selected tab index
    *   onChange: on selected tab index change
    *    tabLabels: array of labels for the tabs
    */
    constructor(props)
    {
        super(props);

        this.id = _.clone(this.props.id);
        if (this.id === undefined || this.id === null)
        {
            this.id = generateId(16);
        }
    }

    render = () => {
        const children = React.Children.toArray(this.props.children);

        return (
            <div className="sleekTabs">
                <div className="tabHeader">
                    <Droppable
                        droppableId={"dropzone_" + this.id}
                    >
                        {(provided, snapshot) => 
                            <div
                                ref={provided.innerRef}
                                style={getListStyle(snapshot.isDraggingOver)}
                                {...provided.droppableProps}
                            >
                                <div className='tabContainer'>
                                    <LeanTabs
                                        value={this.props.value}
                                        onChange={(e, tabIndex) => {this.props.onChange(tabIndex)}} 
                                    >
                                        {this.props.tabLabels.map((label, i) => {return (
                                            <Draggable key={label + i} draggableId={'' + i} index={i}>
                                                {(prov, snap) => 
                                                    <div
                                                        ref={prov.innerRef}
                                                        className="tabHead"
                                                        {...prov.draggableProps}
                                                        {...prov.dragHandleProps}
                                                        style={getItemStyle(snap.isDragging), prov.draggableProps.style}
                                                        onClick={() => {this.props.onChange(i)}}
                                                    >
                                                    {(() => {
                                                        return label;
                                                    })()}
                                                    </div>
                                                }
                                            </Draggable>
                                        )})}
                                    </LeanTabs>
                                </div>
                                {provided.placeholder}
                            </div>
                        }
                    </Droppable>
                </div>
                <div className={'tabPanelContainer'}>
                    {children}
                </div>
            </div>
        );
    }
}

export {SleekTabs};

export function TabPanel(props)
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