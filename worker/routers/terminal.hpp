#pragma once

#include "router_base.hpp"
#include "../config.hpp"

#include <memory>

namespace Routers
{
    class Terminal : public BasicRouter
    {
    public:
        Terminal(CommunicationCenter* collection, attender::http_server& server, Config const& config);
        ~Terminal();

    private:
        void registerRoutes(attender::http_server& server);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
