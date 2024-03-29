#include "terminal.hpp"

namespace Routers
{
//#####################################################################################################################
    struct Terminal::Implementation
    {
        Config config;

        Implementation(Config const& config)
            : config{config}
        {
        }
    };
//#####################################################################################################################
    Terminal::Terminal(CommunicationCenter* collection, attender::http_server& server, Config const& config)
        : BasicRouter{collection, &server}
        , impl_{new Terminal::Implementation(config)}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    Terminal::~Terminal() = default;
//---------------------------------------------------------------------------------------------------------------------
    void Terminal::registerRoutes(attender::http_server& server)
    {
        cors_options(server, "/api/terminal/submit", "POST", impl_->config.corsOption);
        server.post("/api/terminal/submit", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                auto sess = this_session(req);

                if (!body.contains("path"))
                    return respondWithError(res, "need path in json body");
            });
        });
    }
//#####################################################################################################################
}
