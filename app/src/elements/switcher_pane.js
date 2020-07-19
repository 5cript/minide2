import React from 'react';
import classNames from 'classnames';

// Style
import './styles/switcher_pane.css';

class SwitcherPane extends React.Component
{
    state = {
        switchState: true,
        position: 0
    }

    forceSwitch()
    {
        this.setState({
            switchState: !this.state.switchState
        });
    }

    onSwitchClick(to)
    {
        let denySwitch = false;
        if (this.props.onSwitch !== undefined) 
        {
            denySwitch = (this.props.onSwitch(to) === false);
        }

        if (denySwitch)
            return;

        this.setState({
            switchState: !this.state.switchState
        });
    }

    render() 
    {
        const children = React.Children.toArray(this.props.children).slice(0, 2);
        while (children.length < 2)
            children.push(<div />);

        return (
            <div className={classNames("sliderContainer", this.props.className ? this.props.className : '')}>
                <div 
                    className={classNames("leftPane", this.state.switchState ? "leftPaneRetracted" : "leftPaneReversal")}
                >
                    <div
                        className={classNames("paneButton", "leftPaneButton")}
                        onClick={()=>{this.onSwitchClick("toLeft")}}
                    >
                        <div className="offsetter">
                            <i className="arrow switcher_left"></i>
                        </div>
                    </div>
                    {children[1]}
                </div>
                <div 
                    className={classNames("rightPane", !this.state.switchState ? "rightPaneRetracted" : "rightPaneReversal")}
                >
                    {children[0]}
                    <div
                        className={classNames("paneButton", "rightPaneButton")}
                        onClick={()=>{this.onSwitchClick("toRight")}}
                    >
                        <div className="offsetter">
                            <i className="arrow switcher_right"></i>
                        </div>
                    </div>
                </div>
            </div>
        );
    }
};

export default SwitcherPane;