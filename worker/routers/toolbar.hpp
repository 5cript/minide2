#pragma once

#include "router_base.hpp"

#include "../toolbars/basic_toolbar.hpp"
#include "../streaming/streamer_base.hpp"
#include "../routers/settings_provider_fwd.hpp"
#include "../config.hpp"

#include <memory>

namespace Routers
{
    class Toolbar : public BasicRouter
    {
    public:
        Toolbar(CommunicationCenter* collection, attender::http_server& server, Config const& config);
        ~Toolbar();

    private:
        void registerRoutes(attender::http_server& server);
        void loadToolbars
        (
            Session& session,
            std::string const& id,
            Streaming::StreamerBase* streamer,
            SettingsProvider* settingsProv
        );

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
