import React from 'react'
import {connect} from 'react-redux';

// Components
import RingLoader from "react-spinners/RingLoader";

// Util
import { css } from "@emotion/core";

// Style
import './styles/toolbar_blocker.css';

const override = css`
  display: block;
  margin: 0;
  top: calc(calc(calc(var(--toolbar-height) - 30px) / 2) - 4px);
  left: 35px;
  border-color: red;
`;

class Blocker extends React.Component 
{
    render()
    {
        return (
            <div id='ToolbarBlocker'>
                <div id='Stripes'></div>
                <div id='ConnectArea'>
                    <RingLoader
                        size={30}
                        css={override}
                        className='loader'
                        color={"var(--theme-color)"}
                        loading={this.props.backend.tryingToConnect}
                    />
                    <div id='ConnectSign'>{this.props.backend.connectMessage}</div>
                </div>
            </div>
        );
    }
};


export default connect(state => {
    return {
        backend: state.backend
    }
})(Blocker);