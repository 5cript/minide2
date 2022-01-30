#pragma once

#include "session_fwd.hpp"
#include "session.hpp"
#include "session_storage.hpp"

#include <attender/session/session_manager.hpp>

#include <memory>

/**
 *  Allows to retrieve a session. Intentionally doesn't let you save.
 *  The reason is simple: saving while a request is in progress that saves the session
 *  will cause a race condition over who saves first. Its safe in the way that it cannot segfault, but
 *  one of the saves is discarded.
 */
class SessionObtainer
{
public:
    SessionObtainer(std::weak_ptr <attender::session_manager> sessionManager, std::string id);
    bool reload() const;
    std::optional <Session> session(bool forceReload = false) const;

private:
    std::weak_ptr <attender::session_manager> sessionManager_;
    std::string id_;
    mutable std::optional <Session> memorizedSession_;
};
