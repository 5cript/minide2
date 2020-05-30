#pragma once

#include "router_base.hpp"

#include <memory>

namespace Routers
{
    class Terminal : public BasicRouter
    {
    public:
        Terminal(RouterCollection* collection, attender::tcp_server& server);

    private:
        void registerRoutes(attender::tcp_server& server);

    private:
        //std::vector <std::unique_ptr <Toolbars::BasicToolbar>> toolbars;
    };
}
