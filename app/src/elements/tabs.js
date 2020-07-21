import React from 'react';

// components
import Tabs from '@material-ui/core/Tabs';
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
        width: '100%',
        minHeight: 0,
        margin: 0,
        backgroundColor: 'var(--background-color)'
    },
    indicator: {
        backgroundColor: 'var(--theme-color-brighter)',
    },
})(Tabs);

const getListStyle = isDraggingOver => ({
    display: 'flex',
    paddingTop: '2px',
    paddingBot: '0px',
    paddingLeft: '0px',
    overflow: 'auto',
    margin: '0px',
    width: '100%'
});

const getItemStyle = (isDragging, isSelected, draggableStyle) => ({
    // some basic styles to make the items look a bit nicer
    // change background colour if dragging
    backgroundColor: isDragging ? 'transparent' : (isSelected ? 'var(--background-color-very-dark)' : undefined),
    color: isSelected ? 'var(--theme-color)' : undefined,
    height: '25px',
  
    // styles we need to apply on draggables
    ...draggableStyle,
  });
  

class MuiTabs extends React.Component
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
        const value = this.props.value;

        return (
            <div className="sleekTabs">
                <div className="tabHeader">
                    <Droppable
                        droppableId={"dropzone_" + this.id}
                        direction='horizontal'
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
                                        TabIndicatorProps={{
                                            style: {
                                                display: snapshot.isDraggingOver ? 'none' : undefined
                                            }
                                        }}
                                    >
                                        {this.props.tabLabels.map((label, i) => {return (
                                            <Draggable key={label + i} draggableId={"dropzone_" + this.id + "_" + i} index={i}>
                                                {(prov, snap) => 
                                                    <div
                                                        ref={prov.innerRef}
                                                        className="tabHead"
                                                        {...prov.draggableProps}
                                                        {...prov.dragHandleProps}
                                                        style={getItemStyle(snap.isDragging, value === i, prov.draggableProps.style)}
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
};

export {MuiTabs};

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
            {...other}
        >
            {children}
        </div>
    )
}