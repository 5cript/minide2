#include "temporary_session.hpp"
#include "session_storage.hpp"

#include <attender/session/uuid_session_cookie_generator.hpp>

//#####################################################################################################################
namespace
{
    template <typename ServerT>
    void setSession(ServerT& server, Session const& s)
    {
        auto* manager = server.get_session_manager();
        if (manager != nullptr)
            manager->save_session(s);
        else
            throw std::runtime_error("session control not installed");
    }

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
