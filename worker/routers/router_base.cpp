#include "router_base.hpp"

#include "../json.hpp"

namespace Routers
{
//#####################################################################################################################
    BasicRouter::BasicRouter(RouterCollection* collection)
        : collection_{collection}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void BasicRouter::respondWithError(attender::response_handler* res, int status, char const* msg)
    {
        res->status(status);
        jsonResponse(res, json {
            {"error", true},
            {"message", msg}
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void BasicRouter::readExcept(boost::system::error_code ec)
    {
        std::cout << "error during body read: " << ec << "\n";
    }
//#####################################################################################################################
}
