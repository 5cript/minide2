#include "debugger.hpp"

namespace Routers
{
//#####################################################################################################################
    struct Debugger::Implementation
    {
        Config config;

        Implementation( Config const& config)
            : config{config}
        {
        }
    };
//#####################################################################################################################
    Debugger::Debugger(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter(collection, &server)
        , impl_{new Debugger::Implementation(config)}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    Debugger::~Debugger()
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void Debugger::registerRoutes(attender::tcp_server& server)
    {
        cors_options(server, "/api/debugger/start", "POST", impl_->config.corsOption);
        server.post("/api/debugger/start", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);
        });
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
