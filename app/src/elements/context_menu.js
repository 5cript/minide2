import React from 'react';
import PropTypes from 'prop-types';
import _ from 'lodash';

import './styles/context_menu.css';

const MenuItem = ({ item, anyImage }) => 
{
    const {
        label,
        icon,
        onClick,
        isLine,
        disabled
    } = item;

    if (isLine !== true)
        return (
            <span
                className="menuItem"
                onClick={(e) => {!disabled && onClick(e, item.label)}}
                key={label}
                style={{
                    cursor: (disabled === true) ? "not-allowed" : "pointer",
                    "display":"flex",
                    "alignItems":"center",
                    "justifyContent":"flex-start",
                    "marginBottom":"7px",
                    opacity: (disabled === true) ? 0.5 : undefined
                }}
            >
                {icon && <img className="menuIcon" src={icon} alt={'X'} />}
                {icon === undefined && anyImage && <div className="menuImageFill"></div>}
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
        this.visible = false;
    }

    componentDidMount() {
        const { contextId } = this.props;
        const context = document.getElementById(contextId);
        if (this.props.openOnClick !== true)
            context.addEventListener('contextmenu', (event) => {
                this.openContextMenu(event);
            });
        else
            context.addEventListener('click', (event) => {
                if (this.props.closeOnClick === true && this.visible)
                    this.closeContextMenu();
                else
                    this.openContextMenu(event);
            });

        const menu = document.getElementById(this.menuId);
        menu.addEventListener('mouseleave', () => {
            const { closeOnClickOut, closeOnClick } = this.props;
            if (!closeOnClickOut && !closeOnClick) {
                this.closeContextMenu();
            }
        });

        document.addEventListener('click', (event) => {
            const { closeOnClickOut } = this.props;

            if (closeOnClickOut && !menu.contains(event.target)) 
            {
                event.preventDefault();
                this.closeContextMenu();
            }
        });
    }

    openContextMenu(event) 
    {
        let xOffset = Math.max(document.documentElement.scrollLeft, document.body.scrollLeft);
        let yOffset = Math.max(document.documentElement.scrollTop, document.body.scrollTop);

        let element = this.props.onOpen(xOffset, yOffset, event);
        const x = element.x;
        const y = element.y;

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
                if (menu)
                    menu.style.cssText = menu.style.cssText + 'visibility: hidden;'
            }
        }

        const menu = document.getElementById(this.menuId);

        this.visible = true;
        menu.style.cssText = menu.style.cssText + `left: ${x}px;`
            + `top: ${y}px;`
            + 'visibility: visible;';
    }

    closeContextMenu() 
    {
        const menu = document.getElementById(this.menuId);
        this.visible = false; 
        if (this.props.onClose)
            this.props.onClose();
        if (menu)
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

    anyImage = () => 
    {
        return _.filter(this.props.items, item => item.pngbase64 !== undefined) > 0;
    }

    render() {
        return (
            <div
                id={this.menuId}
                className="contextMenu"
                style={{ 
                    "position": "fixed", 
                    "display": "flex", 
                    "flexFlow": "column", 
                    "border": "1px solid rgba(0,0,0,0.15)", 
                    "boxShadow": "0 1px 1px 1px rgba(0,0,0,0.05)", 
                    "visibility": "hidden",
                    "zIndex": "10"
                }}
            >
                {this.getItems().map(item => (
                    <MenuItem item={item} key={item.label} anyImage={this.anyImage} />
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
    otherMenus: PropTypes.array,
    onClose: PropTypes.func
};

ContextMenu.defaultProps = {
    items: []
};