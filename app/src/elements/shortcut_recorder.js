import React from 'react';
import classnames from 'classnames';
import {SlimButton} from './button';

// styling
import './styles/shortcut_recorder.css';

function keybindRenderer(bindings) 
{
	if (bindings.ctrl)
		bindings.ctrlKey = bindings.ctrl;
	if (bindings.alt)
		bindings.altKey = bindings.alt;
	if (bindings.shift)
		bindings.shiftKey = bindings.shift;
	if (bindings.meta)
		bindings.metaKey = bindings.meta;

	if (bindings.key === undefined)
		return <div>{"Invalid Combination"}</div>

	const createFrame = (title, noplus) => {
		return (
			<div style={{display: "flex"}}>
				<div className="shortcutContainerKeyframe">
					{title}
				</div>
				{noplus === true ? "" : " + "}
			</div>
		)

	}

	return (
		<div className="shortcutContainerKeys">
			{bindings.ctrlKey ? createFrame("Ctrl") : <></>}
			{bindings.shiftKey ? createFrame("Shift") : <></>}
			{bindings.altKey ? createFrame("Alt") : <></>}
			{bindings.metaKey ? createFrame("Meta") : <></>}
			{bindings.key ? createFrame(
				bindings.key.toUpperCase() + 
				(bindings.location === 3 ? ' (Numpad)' : '')
			, true) : <></>}
		</div>
	);
}

class ShortcutRecorder extends React.Component
{
	state = {}

	bindings = {
		key: "",
		ctrlKey: false,
		shiftKey: false,
		altKey: false,
		metaKey: false,
        keyCode: 0,
        location: 0
	}

	getKey = () => 
	{
		return keybindRenderer(this.state);
	}

	keyUp = (e) => 
	{
		e.preventDefault()
		if (e.key === "Alt" || e.key === "Shift" || e.key === "Control" || e.key === "Meta")
			return

		this.setState({
			...this.bindings
		})
		this.bindings = {}
	}

	keyDown = (e) => 
	{
		e.preventDefault()
		let key = e.key

		if (e.key === "Alt" || e.key === "Shift" || e.key === "Control" || e.key === "Meta")
			key = undefined

		this.bindings = {
			key: key ? key : this.bindings.key,
			ctrlKey: this.bindings.ctrlKey || e.ctrlKey,
			shiftKey: this.bindings.shiftKey || e.shiftKey,
			altKey: this.bindings.altKey || e.altKey,
			metaKey: this.bindings.metaKey || e.metaKey,
            keyCode: e.keyCode,
            location: e.location
		}
	}

    render = () =>
    {
		return (
			<div className={classnames("shortcutRecorderContainer", this.props.className)}>
				<div>{this.props.dict.translate("$PressKey", 'keybinds') + ": " + this.props.shortcutName}</div>
				<input autoFocus className="shortcutRecorderInput" onKeyUp={this.keyUp} onKeyDown={this.keyDown}></input>
				<div>{this.getKey()}</div>
				<div className="shortcutRecorderButtons">
					<SlimButton onClick={() => this.props.onAccept(this.state)}>{this.props.dict.translate('$Ok', 'dialog')}</SlimButton>
					<SlimButton onClick={this.props.onClose}>{this.props.dict.translate('$Cancel', 'dialog')}</SlimButton>
				</div>
			</div>
		);
	}
}

export {ShortcutRecorder, keybindRenderer};
export default ShortcutRecorder;