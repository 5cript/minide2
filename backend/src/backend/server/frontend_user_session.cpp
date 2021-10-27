#include "frontend_user_session.hpp"
#include "backend_control.hpp"

#include <iostream>

FrontendUserSession::FrontendUserSession(attender::websocket::connection* owner, std::weak_ptr<BackendControl> server, std::string sessionId)
    : attender::websocket::session_base{owner}
    , server_{server}
    , sessionId_{std::move(sessionId)}
{
}

void FrontendUserSession::on_close() 
{
    std::cout << "session closed\n";
    auto shared = server_.lock();
    if (!shared)
        return;

    shared->removeSession(sessionId_);
}