#pragma once

#include "router_base.hpp"

#include "../toolbars/basic_toolbar.hpp"

#include <memory>

namespace Routers
{
    class Toolbar : public BasicRouter
    {
    public:
        Toolbar(RouterCollection* collection, attender::tcp_server& server);

    private:
        void registerRoutes(attender::tcp_server& server);
        bool parseIds(attender::request_handler* req, attender::response_handler* res, std::size_t& tid, std::size_t& cid);

    private:
        std::vector <std::unique_ptr <Toolbars::BasicToolbar>> toolbars;
    };
}
