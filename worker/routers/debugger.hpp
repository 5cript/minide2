#pragma once

#include "router_base.hpp"

#include "../toolbars/basic_toolbar.hpp"
#include "../routers/settings_provider_fwd.hpp"
#include "../config.hpp"

#include <memory>

namespace Routers
{
    class DebuggerRouter : public BasicRouter
    {
    public:
        DebuggerRouter(CommunicationCenter* collection, attender::http_server& server, Config const& config);
        ~DebuggerRouter();

    private:
        void registerRoutes(attender::http_server& server);
        void addCreateInstanceRoute(attender::http_server& server);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
