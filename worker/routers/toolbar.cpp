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

            loadToolbars
            (
                sess,
                req->get_cookie_value("aSID").value(),
                &collection_->streamer()
            );
            sess.save();

            json toolbars = json::object();
            toolbars["toolbars"] = json::array();
            for (auto const& toolbar : sess.toolbarStore.scriptedToolbars)
            {
                toolbars["toolbars"].push_back(toolbar->getJson());
            }
            sendJson(res, toolbars);
        });

        cors_options(server, "/api/toolbar/callAction", "POST", impl_->config.corsOption);
        server.post("/api/toolbar/callAction", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [req, res, this](json const& body)
            {
                if (!body.contains("toolbarId"))
                    return res->status(400).send("need toolbarId");
                if (!body.contains("itemId"))
                    return res->status(400).send("need itemId");

                auto session = this_session(req);
                auto* toolbar = session.toolbarStore.toolbarById(body["toolbarId"].get<std::string>());
                if (toolbar == nullptr)
                    return res->status(400).send("toolbar with given id not found");

                auto resultMessage = toolbar->clickAction(body["itemId"].get<std::string>());
                if (resultMessage.empty())
                    res->status(200).end();
                else
                    res->status(400).send(resultMessage);
            });
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void Toolbar::loadToolbars(Session& session, std::string const& id, DataStreamer* streamer)
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
                    streamer
                ));
        }
    }
//#####################################################################################################################
}
