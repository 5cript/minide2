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
    Terminal::Terminal(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter{collection, &server}
        , impl_{new Terminal::Implementation(config)}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    Terminal::~Terminal() = default;
//---------------------------------------------------------------------------------------------------------------------
    void Terminal::registerRoutes(attender::tcp_server& server)
    {

    }
//#####################################################################################################################
}
