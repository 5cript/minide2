import React from 'react';
import Slide from 'react-reveal/Slide';

// Style
import './styles/switcher_pane.css';

class SwitcherPane extends React.Component
{
    render() 
    {
        const children = React.Children.toArray(this.props.children).slice(0, 2);
        while (children.length < 2)
            children.push(<div />);

        const wrappedChildren = [];
        for (let i = 0; i != children.length; ++i)
        {
            wrappedChildren.push(
                <div style={{height: "100%"}}>
                    {children[i]}
                </div>
            )
        }

        return(
            <div>
                <Slide left when={false}>
                    <div className="leftSlideContent">
                        <div className="switchButtonLeft"></div>
                        {wrappedChildren[0]}
                    </div>
                </Slide>
                <Slide right when={true}>
                    <div className="rightSlideContent">
                        {wrappedChildren[1]}
                        <div className="switchButtonRight"></div>
                    </div>
                </Slide>
            </div>
        );
    }
};

export default SwitcherPane;