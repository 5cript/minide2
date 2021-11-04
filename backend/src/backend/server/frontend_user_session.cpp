#include <backend/server/frontend_user_session.hpp>
#include <backend/server/backend_control.hpp>
#include <backend/server/api/user.hpp>
#include <backend/server/api/workspace.hpp>
#include <backend/server/writer.hpp>
#include <backend/log.hpp>

#include <backend/plugin_system/toolbar.hpp>
#include <backend/plugin_system/isolate.hpp>
#include <backend/plugin_system/script.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std::chrono_literals;

//#####################################################################################################################
struct FrontendUserSession::Implementation
{
    std::weak_ptr<BackendControl> server;
    std::string sessionId;
    StreamParser textParser;
    Dispatcher dispatcher;
    bool authenticated;
    std::shared_ptr <Writer> activeWriter;
    PluginSystem::Isolate javascriptIsolate;

    // API
    Api::User user;
    Api::Workspace workspace;

    // Toolbars
    std::vector<Toolbar> toolbars;

    Implementation
    (
        std::weak_ptr<BackendControl> server, 
        std::string sessionId
    )
        : server{server}
        , sessionId{std::move(sessionId)}
        , textParser{}
        , dispatcher{}
        , authenticated{false}
        , activeWriter{}
        // API
        , user{}
        , workspace{&dispatcher}
        , toolbars{}
    {}

    void imbueOwner(std::weak_ptr<FrontendUserSession> session)
    {
        workspace.setSession(session);
    }
};
//#####################################################################################################################
FrontendUserSession::FrontendUserSession
(
    attender::websocket::connection* owner, 
    std::weak_ptr<BackendControl> server, 
    std::string sessionId
)
    : attender::websocket::session_base{owner}
    , impl_{std::make_unique<Implementation>(server, sessionId)}
{
}
//---------------------------------------------------------------------------------------------------------------------
FrontendUserSession::~FrontendUserSession() = default;
//---------------------------------------------------------------------------------------------------------------------
FrontendUserSession::FrontendUserSession(FrontendUserSession&&) = default;
//---------------------------------------------------------------------------------------------------------------------
FrontendUserSession& FrontendUserSession::operator=(FrontendUserSession&&) = default;
//---------------------------------------------------------------------------------------------------------------------
void FrontendUserSession::setWriter(std::shared_ptr <Writer> writer)
{
    impl_->activeWriter = std::move(writer);
    impl_->activeWriter->write();
}
//---------------------------------------------------------------------------------------------------------------------
void FrontendUserSession::on_close() 
{
    LOG() << "Session closed\n";
    auto shared = impl_->server.lock();
    if (!shared)
        return;

    shared->removeSession(impl_->sessionId);
}
//---------------------------------------------------------------------------------------------------------------------
void FrontendUserSession::setup()
{
    impl_->imbueOwner(weak_from_this());
    PluginSystem::Script script{impl_->javascriptIsolate, "console.log('hi');"};
    script.run();
}
//---------------------------------------------------------------------------------------------------------------------
void FrontendUserSession::on_text(std::string_view txt) 
{
    impl_->textParser.feed(txt);
    const auto popped = impl_->textParser.popMessage();
    if (!popped)
        return;

    if (!popped->contains("ref"))
        return (void)(LOG() << "Message lacks ref, cannot reply and will not process the request.\n");

    if (!popped->contains("type"))
        return respondWithError((*popped)["ref"].get<int>(), "Type missing in message.");

    LOG() << (*popped)["type"].get<std::string>() << " called\n";

    try 
    {
        onJson(*popped);
    }
    catch(std::exception const& exc)
    {
        LOG() << "Connection was ended by exception: " << exc.what() << "\n";
    }
}
//---------------------------------------------------------------------------------------------------------------------
void FrontendUserSession::onJson(json const& j)
{
    if (impl_->authenticated)
        return impl_->dispatcher.dispatch(j);

    if (impl_->user.authenticate(j["payload"]))
    {
        impl_->authenticated = true;
        writeJson(json{
            {"ref", j["ref"]},
            {"authenticated", true}
        });
    }
}
//---------------------------------------------------------------------------------------------------------------------
void FrontendUserSession::respondWithError(int ref, std::string const& msg)
{
    LOG() << "Responding with error: " << msg << "\n";
    writeJson(json{
        {"ref", ref},
        {"error", msg}
    });
}
//---------------------------------------------------------------------------------------------------------------------
bool FrontendUserSession::writeJson(json const& j, std::function<void(std::size_t)> const& on_complete)
{
    // TODO: Improve me.
    std::string serialized = j.dump();
    std::stringstream sstr;
    sstr << "0x" << std::hex << std::setw(8) << std::setfill('0') << serialized.size() << "|" << serialized;
    return write_text(sstr.str(), on_complete);
}
//---------------------------------------------------------------------------------------------------------------------
bool FrontendUserSession::writeBinary(int ref, std::string const& data, std::size_t amount, std::function<void(std::size_t)> const& on_complete)
{
    // TODO: Improve me.
    const auto size = std::min(data.size(), amount);
    std::string decorated(size + 10, '\0');
    std::stringstream sstr;
    sstr << "0x" << std::hex << std::setw(8) << std::setfill('0') << ref;
    const auto refStr = sstr.str();
    std::copy(std::begin(refStr), std::end(refStr), std::begin(decorated));
    std::copy(std::begin(data), std::begin(data) + size, std::begin(decorated) + 10);
    return write_binary(decorated.c_str(), decorated.size(), on_complete);
}
//---------------------------------------------------------------------------------------------------------------------
void FrontendUserSession::on_write_complete(std::size_t bytesTransferred)
{
    session_base::on_write_complete(bytesTransferred);
}
//---------------------------------------------------------------------------------------------------------------------
void FrontendUserSession::on_binary(char const*, std::size_t) 
{
    LOG() << "on_binary\n";
}
//#####################################################################################################################