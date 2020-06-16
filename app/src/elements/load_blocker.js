import React from 'react';

// Components
import Rodal from 'rodal';
import MoonLoader from "react-spinners/MoonLoader";

// Styles
import './styles/load_blocker.css';

// Util
import { css } from "@emotion/core";

const override = css`
  display: block;
  margin: 0;
  left: calc(50% - 60px);
  border-color: red;
`;

/**
 * Props:
 *  - visible: is visible?
 *  - message: what to display
 *  - onButtonPress: fired when a button is pressed
 */
class LoadBlocker extends React.Component
{
    constructor(props)
    {
        super(props)

        if (this.props.dict === undefined)
            this.dict = {translate: (x) => {return x;}}
        else
            this.dict = this.props.dict;

        if (this.props.boxStyle === undefined)
            this.boxStyle = "Ok";
        else
            this.boxStyle = this.props.boxStyle
    }

    render()
    {
        return (
            <Rodal 
                closeOnEsc={false} 
                showCloseButton={false} 
                visible={this.props.visible} 
                enterAnimation={"fade"}
                leaveAnimation={"fade"}
                className='loadBlockRodal'
                customStyles={{
                    overflow: 'hidden'
                }}
            >
                <MoonLoader
                    size={100}
                    css={override}
                    className='loadBlockAnimation'
                    color={"var(--theme-color)"}
                    loading={true}
                />
                <div className='loadBlockMessage'>{this.props.progressMessage}</div>
            </Rodal>
        );
    }
}

export default LoadBlocker;