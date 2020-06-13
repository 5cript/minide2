import React from 'react';

// Components
import JsonOptions from '../../elements/json_options';

// Other
import Dictionary from '../../util/localization';
//import "../../util/ace_sane_theme";
// https://github.com/securingsincity/react-ace/issues/126#issuecomment-345151567

// Styles
import './styles/environments.css';

// Requires

class Environments extends React.Component 
{
    dict = new Dictionary();

    state = {
        jsonRep: '{}'
    }

    render()
    {
        return (
            <JsonOptions 
                dict={this.dict} 
                json={this.state.jsonRep}
                onJsonUpdate={j => {
                    this.setState({
                        jsonRep: j
                    })
                }}
            >
                <div>myCustomUi</div>
            </JsonOptions>
        );
    }
};

export default Environments