#include "router_base.hpp"

#include "../json.hpp"
#include "../session/session_storage.hpp"

#include <attender/attender/session/uuid_session_cookie_generator.hpp>

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
        res->type(".json");
        jsonResponse(res, json {
            {"error", true},
            {"message", msg}
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void BasicRouter::respondWithError(attender::response_handler* res, std::string msg, json additionalInfo)
    {
        res->status(400);
        res->type(".json");

        additionalInfo["error"] = true;
        additionalInfo["message"] = msg;

        jsonResponse(res, additionalInfo);
    }
//---------------------------------------------------------------------------------------------------------------------
    void BasicRouter::respondWithError(attender::response_handler* res, std::string msg)
    {
        respondWithError(res, msg, json::object());
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
    }
//---------------------------------------------------------------------------------------------------------------------
    void TemporarySession::save()
    {
        setSession(*server_, *this);
    }
//---------------------------------------------------------------------------------------------------------------------
    void TemporarySession::save_partial(std::function <void(Session& toSave, Session const& toReadFrom)> const& extractor)
    {
        auto* manager = server_->get_session_manager();
        if (manager != nullptr)

        manager->get_storage<
            timed_memory_session_storage <attender::uuid_generator, Session>
        >()->partially_save_session(id(), *this, extractor);
    }
//---------------------------------------------------------------------------------------------------------------------
    TemporarySession::~TemporarySession()
    {
        try
        {
            // ...
        }
        catch(...)
        {

        }
    }
//#####################################################################################################################
}
