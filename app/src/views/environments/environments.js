import React from 'react';

// Components
import SwitcherPane from '../../elements/switcher_pane';

class Environments extends React.Component 
{
    render()
    {
        return (
            <SwitcherPane>
                <div>left</div>
                <div>right</div>
            </SwitcherPane>
        )
    }
};

export default Environments