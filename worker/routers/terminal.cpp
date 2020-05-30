#include "terminal.hpp"

namespace Routers
{
//#####################################################################################################################
    Terminal::Terminal(RouterCollection* collection, attender::tcp_server& server)
        : BasicRouter{collection}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void Terminal::registerRoutes(attender::tcp_server& server)
    {

    }
//#####################################################################################################################
}
