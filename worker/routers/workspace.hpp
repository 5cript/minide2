#pragma once

#include "router_base.hpp"

namespace Routers
{
    class Workspace : public BasicRouter
    {
    public:
        Workspace(RouterCollection* collection, attender::tcp_server& server);

    private:
        void registerRoutes(attender::tcp_server& server);

    private:
        struct Implementation;
    };
}
