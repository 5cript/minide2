#include "toolbar.hpp"

#include "../toolbars/cmake_toolbar.hpp"
#include "../variant.hpp"
#include "../filesystem/home_directory.hpp"
#include "../session/session_obtainer.hpp"
#include "../routers.hpp"

#include <nlohmann/json.hpp>
#include <iterator>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

using namespace Toolbars;
using namespace std::string_literals;

namespace Routers
{
//#####################################################################################################################
    struct Toolbar::Implementation
    {
        Config config;

        Implementation(Config const& config)
            : config{config}
        {
        }
    };
//#####################################################################################################################
    namespace
    {
        template <typename T>
        void jsonifyActor(json& obj, T actor)
        {
            std::visit([&obj](auto&& act) {
                obj["type"] = act.type;
                obj["id"] = act.id;
                obj["helpText"] = act.helpText;
            }, actor);
        }
    }
//#####################################################################################################################
    Toolbar::Toolbar(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter{collection, &server}
        , impl_{new Toolbar::Implementation(config)}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    Toolbar::~Toolbar() = default;
//---------------------------------------------------------------------------------------------------------------------
    void Toolbar::registerRoutes(attender::tcp_server& server)
    {
        using namespace Toolbars::Types;

        cors_options(server, "/api/toolbar/loadAll", "POST", impl_->config.corsOption);
        server.post("/api/toolbar/loadAll", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto sess = this_session(req);
            //sess.toolbarStore.scriptedToolbars

            try
            {
                loadToolbars
                (
                    sess,
                    req->get_cookie_value("aSID").value(),
                    &collection_->streamer(),
                    &collection_->settingsProv()
                );
                sess.save_partial([](auto& toSave, auto& from)
                {
                    toSave.toolbarStore = from.toolbarStore;
                });

                json toolbars = json::object();
                toolbars["toolbars"] = json::array();
                for (auto const& toolbar : sess.toolbarStore.scriptedToolbars)
                {
                    toolbars["toolbars"].push_back(toolbar->getJson());
                }
                sendJson(res, toolbars);
            }
            catch(std::exception const& exc)
            {
                respondWithError(res, 400, exc.what());
            }
        });

        cors_options(server, "/api/toolbar/callAction", "POST", impl_->config.corsOption);
        server.post("/api/toolbar/callAction", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                if (!body.contains("toolbarId"))
                    return respondWithError(res, "need toolbarId");
                if (!body.contains("itemId"))
                    return respondWithError(res, "need itemId");

                auto session = this_session(req);
                auto* toolbar = session.toolbarStore.toolbarById(body["toolbarId"].get<std::string>());
                if (toolbar == nullptr)
                    return respondWithError(res, "toolbar with given id not found");

                auto resultMessage = toolbar->clickAction(body["itemId"].get<std::string>());
                if (resultMessage.empty())
                    res->status(200).end();
                else
                    respondWithError(res, resultMessage);
            });
        });

        cors_options(server, "/api/toolbar/menuAction", "POST", impl_->config.corsOption);
        server.post("/api/toolbar/menuAction", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                if (!body.contains("toolbarId"))
                    return respondWithError(res, "need toolbarId");
                if (!body.contains("itemId"))
                    return respondWithError(res, "need itemId");
                if (!body.contains("menuEntryLabel"))
                    return respondWithError(res, "need menuEntryLabel");

                auto session = this_session(req);
                auto* toolbar = session.toolbarStore.toolbarById(body["toolbarId"].get<std::string>());
                if (toolbar == nullptr)
                    return respondWithError(res, "toolbar with given id not found");

                auto resultMessage = toolbar->menuAction
                (
                    body["itemId"].get<std::string>(),
                    body["menuEntryLabel"].get<std::string>()
                );
                if (resultMessage.empty())
                    res->status(200).end();
                else
                    respondWithError(res, resultMessage);
            });
        });

        cors_options(server, "/api/toolbar/loadCombobox", "POST", impl_->config.corsOption);
        server.post("/api/toolbar/loadCombobox", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                if (!body.contains("toolbarId"))
                    return respondWithError(res, "need toolbarId");
                if (!body.contains("itemId"))
                    return respondWithError(res, "need itemId");

                auto session = this_session(req);
                auto* toolbar = session.toolbarStore.toolbarById(body["toolbarId"].get<std::string>());
                if (toolbar == nullptr)
                    return respondWithError(res, "toolbar with given id not found: "s + body["toolbarId"].get<std::string>());

                auto resultMessage = toolbar->loadCombobox
                (
                    body["itemId"].get<std::string>()
                );
                if (resultMessage.empty())
                    res->status(200).end();
                else
                    respondWithError(res, resultMessage);
            });
        });

        cors_options(server, "/api/toolbar/comboboxSelect", "POST", impl_->config.corsOption);
        server.post("/api/toolbar/comboboxSelect", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                if (!body.contains("toolbarId"))
                    return respondWithError(res, "need toolbarId");
                if (!body.contains("itemId"))
                    return respondWithError(res, "need itemId");
                if (!body.contains("selected"))
                    return respondWithError(res, "need selected");

                auto session = this_session(req);
                auto* toolbar = session.toolbarStore.toolbarById(body["toolbarId"].get<std::string>());
                if (toolbar == nullptr)
                    return respondWithError(res, "toolbar with given id not found");

                auto resultMessage = toolbar->comboboxSelect
                (
                    body["itemId"].get<std::string>(),
                    body["selected"].get<std::string>()
                );
                if (resultMessage.empty())
                    res->status(200).end();
                else
                    respondWithError(res, resultMessage);
            });
        });

        cors_options(server, "/api/toolbar/logDoubleClick", "POST", impl_->config.corsOption);
        server.post("/api/toolbar/logDoubleClick", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                if (!body.contains("toolbarId"))
                    return respondWithError(res, "need toolbarId");

                std::string logName = "";
                if (body.contains("logName"))
                    logName = body["logName"].get<std::string>();
                else
                    return respondWithError(res, "need logName");

                int lineNumber = -1;
                if (body.contains("lineNumber"))
                    lineNumber = body["lineNumber"].get<int>();
                else
                    return respondWithError(res, "need lineNumber");

                std::string lineString;
                if (body.contains("lineString"))
                    lineString = body["lineString"].get<std::string>();
                else
                    return respondWithError(res, "need lineString");

                auto session = this_session(req);
                auto* toolbar = session.toolbarStore.toolbarById(body["toolbarId"].get<std::string>());
                if (toolbar == nullptr)
                    return respondWithError(res, "toolbar with given id not found");

                auto result = toolbar->onLogDoubleClick(logName, lineNumber, lineString);
                if (result.empty())
                    return res->status(204).end();
                res->status(200).send(result);
            });
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void Toolbar::loadToolbars(Session& session, std::string const& id, DataStreamer* streamer, SettingsProvider* settingsProv)
    {
        auto toolbars = inHomeDirectory() / "toolbars";
        session.toolbarStore.reset();

        for (auto const& p : sfs::directory_iterator{toolbars})
        {
            if (sfs::is_directory(p) && sfs::exists(p.path() / "main.lua"))
                session.toolbarStore.scriptedToolbars.emplace_back(std::make_shared <ScriptedToolbar>
                (
                    p.path(),
                    SessionObtainer(server_->get_installed_session_manager(), id),
                    streamer,
                    settingsProv,
                    impl_->config
                ));
        }
    }
//#####################################################################################################################
}
