#pragma once

#include "routers/streamer.hpp"
#include "routers/terminal.hpp"
#include "routers/toolbar.hpp"
#include "routers/workspace.hpp"
#include "routers/settings_provider.hpp"

#include "config.hpp"

#include <attender/attender/tcp_fwd.hpp>

class RouterCollection
{
public:
    RouterCollection(attender::tcp_server* server, Config const& config);
    ~RouterCollection();

    void endStreaming();

    /**
     *  Get streamer interface
     */
    Routers::DataStreamer& streamer();

private:
    attender::tcp_server* server_;
    Routers::Workspace workspace_;
    Routers::Toolbar toolbar_;
    Routers::DataStreamer streamer_;
    Routers::Terminal terminal_;
    Routers::SettingsProvider settingsProvider_;
};
