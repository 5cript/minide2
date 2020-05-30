#include "workspace.hpp"

#include "../routers.hpp"
#include "../workspace/workspace.hpp"

#include "../json.hpp"

#include "../streaming/common_messages/binary_data.hpp"

using namespace std::string_literals;

namespace Routers
{
//#####################################################################################################################
    struct Workspace::Implementation
    {
        WorkspaceInfo info;
    };
//#####################################################################################################################
    Workspace::Workspace(RouterCollection* collection, attender::tcp_server& server)
        : BasicRouter{collection}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    void Workspace::registerRoutes(attender::tcp_server& server)
    {
        server.get("/api/workspace/info", [this](auto req, auto res)
        {
            auto data = std::make_shared <std::string>();
            req->read_body(*data).then(
                [data, res, addr{req->ip()}, this]()
                {
                    try
                    {
                        auto j = json::parse(*data);

                        auto result = collection_->streamer().send
                        (
                            StreamChannel::Control,
                            addr,
                            j["id"].get<int>(),
                            new Streaming::Messages::BinaryData(0)
                        );
                        if (result != 0)
                            res->status(400).send("please first listen to the control stream or provide correct listener id");
                        else
                            res->status(204).end();
                    }
                    catch(std::exception const& exc)
                    {
                        res->status(400).send(exc.what());
                    }
                }
            );
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
