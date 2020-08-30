#pragma once

#include "router_base.hpp"

#include "../toolbars/basic_toolbar.hpp"
#include "../routers/streamer.hpp"
#include "../routers/settings_provider_fwd.hpp"
#include "../config.hpp"

#include <memory>

namespace Routers
{
    class Debugger : public BasicRouter
    {
    public:
        Debugger(RouterCollection* collection, attender::tcp_server& server, Config const& config);
        ~Debugger();

    private:
        void registerRoutes(attender::tcp_server& server);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
