#include <backend/server/frontend_user_session.hpp>
#include <backend/server/backend_control.hpp>
#include <backend/server/api/user.hpp>
#include <backend/server/api/workspace.hpp>
#include <backend/server/writer.hpp>
#include <backend/filesystem/home_directory.hpp>
#include <backend/log.hpp>

#include <backend/plugin_system/plugin.hpp>
#include <v8wrap/isolate.hpp>
#include <v8wrap/module.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std::chrono_literals;

namespace Backend::Server
{
    //#####################################################################################################################
    struct FrontendUserSession::Implementation
    {
        std::weak_ptr<BackendControl> server;
        std::string sessionId;
        Stream::StreamParser textParser;
        Stream::Dispatcher dispatcher;
        bool authenticated;
        std::shared_ptr<Writer> activeWriter;
        v8wrap::Isolate javascriptIsolate;
        std::weak_ptr<FrontendUserSession> session;

        // API
        Api::User user;
        Api::Workspace workspace;

        // Toolbars
        std::vector<PluginSystem::Plugin> plugins;

        Implementation(std::weak_ptr<BackendControl> server, std::string sessionId);

        void imbueOwner(std::weak_ptr<FrontendUserSession> session);

        // may throw
        void loadPlugins();
    };
    //---------------------------------------------------------------------------------------------------------------------
    FrontendUserSession::Implementation::Implementation(std::weak_ptr<BackendControl> server, std::string sessionId)
        : server{server}
        , sessionId{std::move(sessionId)}
        , textParser{}
        , dispatcher{}
        , authenticated{false}
        , activeWriter{}
        , user{}
        , workspace{&dispatcher}
        , plugins{}
    {}
    //---------------------------------------------------------------------------------------------------------------------
    void FrontendUserSession::Implementation::imbueOwner(std::weak_ptr<FrontendUserSession> sess)
    {
        session = sess;
        workspace.setSession(sess);
    }
    //---------------------------------------------------------------------------------------------------------------------
    void FrontendUserSession::Implementation::loadPlugins()
    {
        auto pluginDir = inHomeDirectory() / PluginSystem::Plugin::pluginsDir;
        for (sfs::directory_iterator iter{pluginDir}, end; iter != end; ++iter)
        {
            if (sfs::is_directory(iter->status()))
            {
                auto& plugin = plugins.emplace_back(
                    iter->path().filename().string(),
                    Api::AllApis{
                        .workspace = &workspace,
                        .user = &user,
                    });
                plugin.run(session);
            }
        }
    }
    //#####################################################################################################################
    FrontendUserSession::FrontendUserSession(
        attender::websocket::connection* owner,
        std::weak_ptr<BackendControl> server,
        std::string sessionId)
        : attender::websocket::session_base{owner}
        , impl_{std::make_unique<Implementation>(server, sessionId)}
    {}
    //---------------------------------------------------------------------------------------------------------------------
    FrontendUserSession::~FrontendUserSession() = default;
    //---------------------------------------------------------------------------------------------------------------------
    Stream::Dispatcher& FrontendUserSession::getSubscriptionDispatcher()
    {
        return impl_->dispatcher;
    }
    //---------------------------------------------------------------------------------------------------------------------
    void FrontendUserSession::setWriter(std::shared_ptr<Writer> writer)
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
        catch (std::exception const& exc)
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
            writeJson(
                json{
                    {"ref", j["ref"]},
                    {"authenticated", true},
                },
                [this](auto, auto) {
                    onAfterAuthentication();
                });
        }
    }
    //---------------------------------------------------------------------------------------------------------------------
    void FrontendUserSession::endSession()
    {
        close_connection();
    }
    //---------------------------------------------------------------------------------------------------------------------
    void FrontendUserSession::onAfterAuthentication()
    {
        try
        {
            impl_->loadPlugins();
            std::vector<std::string> pluginNames;
            for (auto const& plugin : impl_->plugins)
                pluginNames.push_back(plugin.name());

            writeJson(json{
                {"ref", -1},
                {"pluginsLoaded", pluginNames},
            });
        }
        catch (std::exception const& exc)
        {
            std::cout << exc.what() << "\n";
            writeJson(
                json{
                    {"ref", -1},
                    {"error", exc.what()},
                },
                [this](auto, auto) {
                    endSession();
                });
        }
        catch (...)
        {
            writeJson(
                json{
                    {"ref", -1},
                    {"error", "Non standard exception caught."},
                },
                [this](auto, auto) {
                    endSession();
                });
        }
    }
    //---------------------------------------------------------------------------------------------------------------------
    void FrontendUserSession::respondWithError(int ref, std::string const& msg)
    {
        LOG() << "Responding with error: " << msg << "\n";
        writeJson(json{
            {"ref", ref},
            {"error", msg},
        });
    }
    //---------------------------------------------------------------------------------------------------------------------
    bool FrontendUserSession::writeText(
        std::string const& txt,
        std::function<void(session_base*, std::size_t)> const& on_complete)
    {
        std::stringstream sstr;
        sstr << "0x" << std::hex << std::setw(8) << std::setfill('0') << txt.size() << "|" << txt;
        return write_text(sstr.str(), on_complete);
    }
    //---------------------------------------------------------------------------------------------------------------------
    bool
    FrontendUserSession::writeJson(json const& j, std::function<void(session_base*, std::size_t)> const& on_complete)
    {
        const std::string serialized = j.dump();
        return writeText(serialized, on_complete);
    }
    //---------------------------------------------------------------------------------------------------------------------
    bool FrontendUserSession::writeBinary(
        int ref,
        std::string const& data,
        std::size_t amount,
        std::function<void(session_base*, std::size_t)> const& on_complete)
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
}