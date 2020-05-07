#pragma once
#include <attender/attender.hpp>

namespace Routers
{
    class Workspace
    {
    public:
        Workspace(attender::tcp_server& server);

    private:
        void registerRoutes(attender::tcp_server& server);

    private:
        struct Implementation;
    };
}
