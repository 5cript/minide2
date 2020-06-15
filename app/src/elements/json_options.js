import React from 'react';

// Components
import AceEditor from "react-ace";
import SwitcherPane from './switcher_pane';
import MessageBox from "./message_box";

// Other
import _ from 'lodash';

// ACE
import "ace-builds/webpack-resolver";
import "ace-builds/src-noconflict/mode-json";
import "ace-builds/src-noconflict/theme-clouds_midnight";

class JsonOptions extends React.Component 
{
    constructor(props)
    {
        super(props);
        this.state = {
            editorText: this.props.json,
            boxVisible: false,
            boxMessage: '',
            lastKnownCorrect: this.props.json.length === 0 ? '{}' : this.props.json
        }
    }

    onJsonChange(text)
    {
        this.setState({
            editorText: text
        });
    }

    setEditorRef = editor => 
    {
        this.editor = editor;
    }

    onSwitch(to)
    {
        if (to !== "toLeft")
        {
            this.setState({editorText: this.props.json});
            return true;
        }
        try
        {
            const cpy = _.clone(this.state.editorText);
            JSON.parse(cpy);
            this.onJsonUpdate(cpy);
            return true;
        }
        catch(e)
        {
            let res = false;
            this.showBox(this.props.dict.translate("$JsonBrokenDiscard", "json_options") + " -> " + e.message, () => {
                this.setState({
                    editorText: this.state.lastKnownCorrect
                });
                this.switcher.forceSwitch();
            });
            return res;
        }
    }

    onJsonUpdate(json)
    {
        this.setState({
            lastKnownCorrect: json
        });
        this.props.onJsonUpdate(json);
    }

    showBox(message, action) 
    {
        this.setState({
            boxVisible: true,
            boxMessage: message
        })
        this.yesAction = action;
    }

    onMessageBoxClose(whatButton)
    {
        this.setState({
            boxVisible: false
        });
        if (whatButton === "Yes")
            this.yesAction();
    }

    setSwitchRef = r => {
        this.switcher = r;
    }

    render()
    {
        const children = React.Children.toArray(this.props.children).slice(0, 1);
        while (children.length < 1)
            children.push(<div />);

        return (
            <div className="switcher">
                <SwitcherPane 
                    onSwitch={to=>{return this.onSwitch(to)}}
                    ref={this.setSwitchRef}
                >
                    <div className="fancyRepresentationContainer">
                        {children[0]}
                    </div>
                    <div className="jsonHolder">
                        {this.props.dict.translate("$JsonRepresentation", "json_options")}
                        <AceEditor
                            ref={this.setEditorRef}
                            height="100%"
                            width="100%"
                            theme="clouds_midnight"
                            mode="json"
                            className="jsonEditor"
                            onChange={e => {this.onJsonChange(e)}}
                            name="UniqueAceID"
                            value={this.state.editorText}
                            editorProps={{ $blockScrolling: true }}
                        />
                    </div>
                </SwitcherPane>
                <MessageBox boxStyle="YesNo" dict={this.props.dict} visible={this.state.boxVisible} message={this.state.boxMessage} onButtonPress={(wb)=>{this.onMessageBoxClose(wb);}}/>
            </div>
        )
    }
    
}

export default JsonOptions;