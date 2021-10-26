import {debuggerToggleBreakpoint} from '../../actions/debugging_actions';
import _ from 'lodash';

class GutterControl
{
    constructor(store, editorComponent)
    {
        this.store = store;
        this.editorComponent = editorComponent;
        this.editor = editorComponent.editor;
        this.monaco = editorComponent.monaco;
    }

    onMouseDown = (event) => 
    {
        const line = event.target.range.startLineNumber;

        this.toggleBreakpoint(line);
    }

    removeBreakpoint = (line) =>
    {
        const debuggingState = _.cloneDeep(this.store.getState().debugging);
        let breakpoints = this.getBreakpoints(debuggingState);

        const index = breakpoints.findIndex(breakpoint => {
            return breakpoint.file === this.editorComponent.currentPath &&
                   breakpoint.line === line;
        });
        if (index === -1)
            return;
        else
            breakpoints.splice(index, 1);

        if (debuggingState.focussedInstance)
            this.store.dispatch(debuggerToggleBreakpoint(
                debuggingState.instances[debuggingState.focussedInstance].sessionData, this.editorComponent.currentPath, line
            ));
        else
            this.store.dispatch(debuggerToggleBreakpoint(debuggingState.activeSessionData, this.editorComponent.currentPath, line));

        return breakpoints;
    }

    toggleBreakpoint = (line) =>
    {
        const debuggingState = _.cloneDeep(this.store.getState().debugging);
        let breakpoints = this.getBreakpoints(debuggingState);
        const index = breakpoints.findIndex(breakpoint => {
            return breakpoint.file === this.editorComponent.currentPath &&
                   breakpoint.line === line;
        });
        if (index === -1)
            breakpoints.push({file: this.editorComponent.currentPath, line});
        else
            breakpoints.splice(index, 1);

        if (debuggingState.focussedInstance)
            this.store.dispatch(debuggerToggleBreakpoint(
                debuggingState.instances[debuggingState.focussedInstance].sessionData, this.editorComponent.currentPath, line
            ));
        else
            this.store.dispatch(debuggerToggleBreakpoint(debuggingState.activeSessionData, this.editorComponent.currentPath, line));

        return breakpoints;
    }

    onMouseMove = (event) =>
    {

    }

    getBreakpoints = (debuggingState) => {
        let breakpoints = []
        const file = this.editorComponent.currentPath;
        if (debuggingState.focussedInstance)
            breakpoints = debuggingState.sessionData[debuggingState.instances[debuggingState.focussedInstance].sessionData].breakpoints;
        else
            breakpoints = debuggingState.sessionData[debuggingState.activeSessionData].breakpoints;
        return breakpoints;
    }
};

export default GutterControl;