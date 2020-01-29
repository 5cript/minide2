#include "workspace.hpp"

namespace Routers
{
//#####################################################################################################################
    Workspace::Workspace(attender::tcp_server& server)
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    void Workspace::registerRoutes(attender::tcp_server& server)
    {
        server.get("/api/workspace/info", [this](auto req, auto res)
        {
            std::cout << "hello\n";
            res->send_status(200);
        });

        server.get("/api/workspace/enum")
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
