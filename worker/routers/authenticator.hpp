#pragma once

#include "router_base.hpp"

#include "../toolbars/basic_toolbar.hpp"
#include "../routers/settings_provider_fwd.hpp"
#include "../config.hpp"

#include <memory>

namespace Routers
{
    class Authenticator : public BasicRouter
    {
    public:
        Authenticator(CommunicationCenter* collection, attender::http_server& server, Config const& config);

    private:
        void registerRoutes(attender::http_server& server);

    private:
        Config config_;
    };
}
