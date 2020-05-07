#include "workspace.hpp"

#include "../workspace/workspace.hpp"


namespace Routers
{
//#####################################################################################################################
    struct Workspace::Implementation
    {
        WorkspaceInfo info;
    };
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

        /**
         *  Projects are folders within the workspace.
         */
        server.get("/api/workspace/enum_projects", [this](auto req, auto res)
        {
            res->end();
        });
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
