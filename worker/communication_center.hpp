#pragma once

#include "routers/terminal.hpp"
#include "routers/http_streamer.hpp"
#include "routers/toolbar.hpp"
#include "routers/workspace.hpp"
#include "routers/settings_provider.hpp"
#include "routers/debugger.hpp"
#include "routers/authenticator.hpp"

#include "streaming/ws_streamer.hpp"

#include "config.hpp"

#include <attender/http/http_fwd.hpp>

class CommunicationCenter
{
public:
    CommunicationCenter(attender::http_server* server, Config const& config);
    ~CommunicationCenter();

    /**
     *  Still here for transition period.
     */
    Routers::DataStreamer& httpStreamer();

    /**
     *  Get streamer interface
     */
    Streaming::WebsocketStreamer& streamer();
    Routers::SettingsProvider& settingsProv();

private:
    attender::http_server* server_;
    Routers::Authenticator authenticator_;
    Routers::Workspace workspace_;
    Routers::Toolbar toolbar_;
    Routers::DataStreamer httpStreamer_;
    Routers::Terminal terminal_;
    Routers::SettingsProvider settingsProvider_;
    Routers::DebuggerRouter debuggerRouter_;
    Streaming::WebsocketStreamer wsStreamer_;
};
