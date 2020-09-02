#include "debugger.hpp"
#include "../debugger/debugger.hpp"

#include <optional>
#include <string>

namespace Routers
{
//#####################################################################################################################
    struct DebuggerRouter::Implementation
    {
        Config config;

        Implementation( Config const& config)
            : config{config}
        {
        }
    };
//#####################################################################################################################
    DebuggerRouter::DebuggerRouter(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter(collection, &server)
        , impl_{new DebuggerRouter::Implementation(config)}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    DebuggerRouter::~DebuggerRouter()
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void DebuggerRouter::registerRoutes(attender::tcp_server& server)
    {
        cors_options(server, "/api/debugger/createInstance", "POST", impl_->config.corsOption);
        server.post("/api/debugger/createInstance", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto session = this_session(req);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                std::optional <std::string> executeable;

                if (body.contains("executeable"))
                    executeable = body["executeable"].get<std::string>();

                if (body.contains("executeable"))
                    executeable = body["executeable"].get<std::string>();

                res->end();
            });
        });
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
