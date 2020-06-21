#include "router_base.hpp"

#include "../json.hpp"

namespace Routers
{
//#####################################################################################################################
    BasicRouter::BasicRouter(RouterCollection* collection, server_type* server)
        : collection_{collection}
        , server_{server}
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
//---------------------------------------------------------------------------------------------------------------------
    TemporarySession BasicRouter::this_session(attender::request_handler* req)
    {
        TemporarySession temp(server_, getSession(*server_, req).value());
        return temp;
    }
//#####################################################################################################################
    TemporarySession::TemporarySession(server_type* server, Session&& sess)
        : Session{std::move(sess)}
        , server_{server}
    {
        sessionLock->lock();
    }
//---------------------------------------------------------------------------------------------------------------------
    TemporarySession::~TemporarySession()
    {
        try
        {
            sessionLock->unlock();
            setSession(*server_, *this);
        }
        catch(...)
        {

        }
    }
//#####################################################################################################################
}
