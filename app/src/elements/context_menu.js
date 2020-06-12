import React from 'react';
import PropTypes from 'prop-types';

import './styles/context_menu.css';

const MenuItem = ({ item }) => 
{
    const {
        label,
        icon,
        onClick,
        isLine
    } = item;

    if (isLine !== true)
        return (
            <span
                className="menuItem"
                onClick={onClick}
                key={label}
                style={{"cursor":"pointer","display":"flex","alignItems":"center","justifyContent":"flex-start","marginBottom":"7px"}}
            >
                {icon && <img className="icon" src={icon} alt={'X'} />}
                {label}
            </span>
        );
    else
        return (
            <div className="menuLine">
                
            </div>
        );
};

MenuItem.propTypes = {
    item: PropTypes.shape({
        label: PropTypes.string.isRequired,
        onClick: PropTypes.func.isRequired,
        icon: PropTypes.string,
    }).isRequired,
};

export default class ContextMenu extends React.PureComponent 
{
    constructor(props) {
        super(props);
        this.menuId = this.props.menuId;
        this.getItems = this.getItems.bind(this);
        this.openContextMenu = this.openContextMenu.bind(this);
        this.closeContextMenu = this.closeContextMenu.bind(this);

        this.state = {
            target: '',
        };
    }

    componentDidMount() {
        const { contextId } = this.props;
        const context = document.getElementById(contextId);
        context.addEventListener('contextmenu', (event) => {
            this.openContextMenu(event);
        });

        const menu = document.getElementById(this.menuId);
        menu.addEventListener('mouseleave', () => {
            const { closeOnClickOut } = this.props;
            if (!closeOnClickOut) {
                this.closeContextMenu();
            }
        });

        document.addEventListener('click', (event) => {
            const { closeOnClickOut } = this.props;

            if (closeOnClickOut && !menu.contains(event.target)) {
                event.preventDefault();
                this.closeContextMenu();
            }
        });
    }

    openContextMenu(event) 
    {
        let xOffset = Math.max(document.documentElement.scrollLeft, document.body.scrollLeft);
        let yOffset = Math.max(document.documentElement.scrollTop, document.body.scrollTop);

        let element = this.props.onOpen(xOffset, yOffset);
        xOffset = element.xOffset;
        yOffset = element.yOffset;

        if (!element.doShow)
            return;
        else
            event.preventDefault();
            
        this.setState({ target: event.target });

        if (this.props.otherMenus !== undefined && this.props.otherMenus.length > 0)
        {
            for (let om of this.props.otherMenus)
            {
                const menu = document.getElementById(om);
                menu.style.cssText = menu.style.cssText + 'visibility: hidden;'
            }
        }

        const menu = document.getElementById(this.menuId);

        menu.style.cssText = menu.style.cssText + `left: ${event.clientX + xOffset}px;`
            + `top: ${event.clientY + yOffset}px;`
            + 'visibility: visible;';
    }

    closeContextMenu() 
    {
        const menu = document.getElementById(this.menuId);
        menu.style.cssText = menu.style.cssText + 'visibility: hidden;';
    }

    getItems() 
    {
        const { items } = this.props;
        let counter = 0;
        return items.map(item => {
            if (!item.line)
                return {
                    ...item,
                    onClick: () => {
                        this.closeContextMenu();
                        item.onClick();
                    },
                }
            else
                return {
                    label: '__line_' + (counter++),
                    isLine: true,
                    onClick: () => {}
                }
        })
    }

    render() {
        return (
            <div
                id={this.menuId}
                className="contextMenu"
                style={{ 
                    "position": "absolute", 
                    "display": "flex", 
                    "flexFlow": "column", 
                    "border": "1px solid rgba(0,0,0,0.15)", 
                    "borderRadius": "2px", 
                    "boxShadow": "0 1px 1px 1px rgba(0,0,0,0.05)", 
                    "visibility": "hidden" 
                }}
            >
                {this.getItems().map(item => (
                    <MenuItem item={item} key={item.label} />
                ))}
            </div>
        );
    }
}

ContextMenu.propTypes = {
    items: PropTypes.arrayOf(PropTypes.shape({
        label: PropTypes.string,
        onClick: PropTypes.func,
        icon: PropTypes.string,
        line: PropTypes.bool
    })),
    contextId: PropTypes.string.isRequired,
    menuId: PropTypes.string.isRequired,
    otherMenus: PropTypes.array
};

ContextMenu.defaultProps = {
    items: []
};