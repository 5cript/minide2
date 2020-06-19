#include "toolbar.hpp"

#include "../toolbars/cmake_toolbar.hpp"
#include "../variant.hpp"
#include "../filesystem/home_directory.hpp"

#include <nlohmann/json.hpp>
#include <iterator>
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

            loadToolbars(sess);
            res->status(200).end();
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void Toolbar::loadToolbars(Session& session)
    {
        auto toolbars = inHomeDirectory() / "toolbars";
        session.toolbarStore.reset();

        for (auto const& p : sfs::directory_iterator{toolbars})
        {
            if (sfs::is_directory(p) && sfs::exists(p.path() / "main.lua"))
                session.toolbarStore.scriptedToolbars.emplace_back(std::make_shared <ScriptedToolbar> (p.path()));
        }
    }
//#####################################################################################################################
}
