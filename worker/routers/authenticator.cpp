#include "authenticator.hpp"

namespace Routers
{
//#####################################################################################################################
    Authenticator::Authenticator(CommunicationCenter* collection, attender::http_server& server, Config const& config)
        : BasicRouter(collection, &server)
        , config_{config}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    void Authenticator::registerRoutes(attender::http_server& server)
    {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        cors_options(server, "/api/authenticate", "GET", config_.corsOption);
        server.get("/api/authenticate", [this, &server](auto req, auto res)
        {
            enable_cors(req, res, config_.corsOption);
            if (server.authenticate_session(req, res))
            {
                json j;
                j["aSID"] = req->get_cookie_value("aSID").value();
                res->status(200).send(j.dump());
            }
        });
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
//#####################################################################################################################
}
