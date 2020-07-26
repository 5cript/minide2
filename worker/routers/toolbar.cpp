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
#include <type_traits>

using json = nlohmann::json;

using namespace Toolbars;
using namespace std::string_literals;

namespace Routers
{
//#####################################################################################################################
    struct Toolbar::Implementation
    {
        Config config;
        attender::tcp_server* server;

        Implementation(Config const& config)
            : config{config}
        {
        }

        template <typename FunctionT, typename RouterT>
        void delegateToolbarMemberCall
        (
            std::string const& url,
            FunctionT func,
            RouterT* router,
            std::vector <std::string> const& checks
        )
        {
            cors_options(*server, url, "POST", config.corsOption);
            server->post(url, [this, router, func, checks](auto req, auto res)
            {
                enable_cors(req, res, config.corsOption);

                readJsonBody(req, res, [req, res, router, func, checks, this](json const& body)
                {
                    for (auto const& check : checks)
                        if (!body.contains(check))
                            return router->respondWithError(res, "json body requires "s + check + " to be present");

                    auto session = router->this_session(req);
                    auto* toolbar = session.toolbarStore.toolbarById(body["toolbarId"].get<std::string>());
                    if (toolbar == nullptr)
                        return router->respondWithError(res, "toolbar with given id not found");

                    auto result = func(body, toolbar);
                    if (!result.didFail())
                    {
                        //result.
                        if constexpr (std::is_same_v <typename decltype(result)::success_type, VoidResult>)
                            res->status(200).end();
                        else
                        {
                            res->status(200);
                            json j = json::object();
                            j["apiResult"] = result.result().value();
                            jsonResponse(res, j);
                        }
                    }
                    else
                        router->respondWithError(res, result.error_value().error_message());
                });
            });

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

        impl_->server = &server;

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

        impl_->delegateToolbarMemberCall("/api/toolbar/callAction", [](json const& body, auto& toolbar)
        {
            return toolbar->clickAction(body["itemId"].get<std::string>());
        }, this,
        {"toolbarId", "itemId"});

        impl_->delegateToolbarMemberCall("/api/toolbar/cancelAction", [](json const& body, auto& toolbar)
        {
            return toolbar->cancelAction(body["itemId"].get<std::string>(), body["force"].get<bool>());
        }, this,
        {"toolbarId", "itemId", "force"});

        impl_->delegateToolbarMemberCall("/api/toolbar/menuAction", [](json const& body, auto& toolbar)
        {
            return toolbar->menuAction
            (
                body["itemId"].get<std::string>(),
                body["menuEntryLabel"].get<std::string>()
            );
        }, this,
        {"toolbarId", "itemId", "menuEntryLabel"});

        impl_->delegateToolbarMemberCall("/api/toolbar/loadCombobox", [](json const& body, auto& toolbar)
        {
            return toolbar->loadCombobox
            (
                body["itemId"].get<std::string>()
            );
        }, this,
        {"toolbarId", "itemId"});

        impl_->delegateToolbarMemberCall("/api/toolbar/comboboxSelect", [](json const& body, auto& toolbar)
        {
            return toolbar->comboboxSelect
            (
                body["itemId"].get<std::string>(),
                body["selected"].get<std::string>()
            );
        }, this,
        {"toolbarId", "itemId", "selected"});

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
                if (!result.didFail())
                    res->status(200).end();
                else
                    respondWithError(res, result.error_value().error_message());
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
