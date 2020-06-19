#pragma once

#include "router_base.hpp"
#include "../config.hpp"

#include <memory>

namespace Routers
{
    class Terminal : public BasicRouter
    {
    public:
        Terminal(RouterCollection* collection, attender::tcp_server& server, Config const& config);
        ~Terminal();

    private:
        void registerRoutes(attender::tcp_server& server);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
