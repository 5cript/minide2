#include "session_obtainer.hpp"

//#####################################################################################################################
SessionObtainer::SessionObtainer(std::weak_ptr <attender::session_manager> sessionManager, std::string id)
    : sessionManager_{std::move(sessionManager)}
    , id_{std::move(id)}
    , memorizedSession_{}
{
}
//---------------------------------------------------------------------------------------------------------------------
bool SessionObtainer::reload() const
{
    auto strongRef = sessionManager_.lock();
    if (!strongRef)
        throw std::runtime_error("session manager died, this should be impossible and indicates a mayor issue.");
    Session temp;
    auto res = strongRef->load_session<Session>(id_, &temp);
    if (res != attender::session_state::live)
        return false;
    memorizedSession_ = std::move(temp);
    return true;
}
//---------------------------------------------------------------------------------------------------------------------
std::optional <Session> SessionObtainer::session(bool forceReload) const
{
    if (!memorizedSession_ || forceReload)
        reload();
    return memorizedSession_;
}
//#####################################################################################################################
