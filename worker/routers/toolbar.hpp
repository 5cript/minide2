#pragma once

#include "router_base.hpp"

#include "../toolbars/basic_toolbar.hpp"
#include "../routers/streamer.hpp"
#include "../config.hpp"

#include <memory>

namespace Routers
{
    class Toolbar : public BasicRouter
    {
    public:
        Toolbar(RouterCollection* collection, attender::tcp_server& server, Config const& config);
        ~Toolbar();

    private:
        void registerRoutes(attender::tcp_server& server);
        void loadToolbars(Session& session, std::string const& id, DataStreamer* streamer);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
