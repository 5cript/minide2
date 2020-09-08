import React from 'react';

class DebuggerSettings extends React.Component
{
    render() {
        // NOTE TO MY FUTURE SELF
        // Allow X amount of debugger configuration and settings
        // give these configs a name that can be accessed from the run config like "${lldb}" if there is a profile called lldb.
        // before creating a new debugger instance, these profiles (the settings) could then be merged into the run config
        // for the server to read.
        // make a selection box for what debugger it is (gdb, lldb).

        // maybe something like:
        // https://cdn.discordapp.com/attachments/745676647184990329/752688417284030544/unknown.png
        return <div>DebuggerSettings</div>
    }
}

export default DebuggerSettings;