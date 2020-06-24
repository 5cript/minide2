#include "scripted_toolbar.hpp"
#include "../scripting_engine/common_state_setup.hpp"
#include "../scripting_engine/script.hpp"
#include "../scripting_engine/process.hpp"
#include "../scripting_engine/project_control.hpp"
#include "../session/session_obtainer.hpp"

#include <attender/attender/session/uuid_session_cookie_generator.hpp>

#include <sol/sol.hpp>

#include <utility>
#include <type_traits>
#include <iostream>

using namespace MinIDE::Scripting;
using namespace std::string_literals;

namespace Toolbars
{
//#####################################################################################################################
    struct ScriptedToolbar::Implementation
    {
        sfs::path toolbarRoot;
        std::shared_ptr <StateCollection> engine;
        json jsonRepresentation;
        std::string id;
        attender::uuid_generator gen;
        bool loaded;

        Implementation(sfs::path const& toolbarRoot)
            : toolbarRoot{toolbarRoot}
            , engine{new StateCollection}
            , jsonRepresentation{}
            , id{}
            , gen{}
            , loaded{false}
        {
        }
    };
//#####################################################################################################################
    ScriptedToolbar::ScriptedToolbar(sfs::path const& root, SessionObtainer const& obtainer)
        : BasicToolbar{""}
        , impl_{new ScriptedToolbar::Implementation(root)}
    {
        initialize(obtainer);
    }
//---------------------------------------------------------------------------------------------------------------------
    ScriptedToolbar::~ScriptedToolbar() = default;
//---------------------------------------------------------------------------------------------------------------------
    json ScriptedToolbar::getJson() const
    {
        return impl_->jsonRepresentation;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string ScriptedToolbar::id() const
    {
        return impl_->id;
    }
//---------------------------------------------------------------------------------------------------------------------
    void ScriptedToolbar::initialize(SessionObtainer const& obtainer)
    {
        auto mainScript = MinIDE::Scripting::Script{impl_->toolbarRoot / "main.lua"};

        loadProcessUtility(impl_->engine);

        {
            std::lock_guard <StateCollection::mutex_type> guard{impl_->engine->globalMutex};
            auto& lua = impl_->engine->lua;

            commonStateSetup(lua, true);
            addToPackagePath(lua, impl_->toolbarRoot);

            // Load APIs
            loadProjectControl(impl_->engine, obtainer);

            lua["debugging"] = false;
            lua.script(mainScript.script());
            sol::table interface = lua["make_toolbar"]();

            impl_->jsonRepresentation["items"] = json::array();
            impl_->jsonRepresentation["name"] = interface["name"].get_or<std::string>("missing_name");
            auto defaultId = impl_->gen.generate_id();
            impl_->id = interface["id"].get_or<std::string>(defaultId);
            impl_->jsonRepresentation["id"] = impl_->id;

            for (sol::table const items = interface["items"]; auto const& [key, value] : items)
            {
                json jsonItem = json::object();
                sol::table item = value;

                auto transferToJson = [&jsonItem, &item](auto id, auto alternate)
                {
                    auto val = item.get_or<std::decay_t<decltype(alternate)>>(id, alternate);
                    if constexpr
                    (
                        std::is_same_v<std::decay_t<decltype(alternate)>, std::string> ||
                        std::is_same_v<std::decay_t<decltype(alternate)>, std::vector <std::string>>
                    )
                    {
                        if (val.empty())
                            return;
                    }
                    jsonItem[id] = val;
                };

                if (!item["type"].valid())
                    continue;

                std::string type = item.get<std::string>("type");
                jsonItem["type"] = type;
                transferToJson("id", "missing_id_"s + key.as<std::string>());

                if (type == "IconButton")
                {
                    transferToJson("pngbase64", ""s);
                    transferToJson("special_actions", std::vector <std::string>{});
                }

                impl_->jsonRepresentation["items"].push_back(jsonItem);
            }

            impl_->loaded = true;
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string ScriptedToolbar::clickAction(std::string const& id)
    {
        std::lock_guard <StateCollection::mutex_type> guard{impl_->engine->globalMutex};
        auto& lua = impl_->engine->lua;

        if (!impl_->loaded)
            return "not loaded (how did this happen?) "s + __FILE__ + ":" + std::to_string(__LINE__);

        sol::table interface = lua["get_toolbar"]();
        sol::table items = interface["items"];

        // there might be a better option, but it wont exceed 100 anyway.
        // might take indices, but is the order guaranteed?
        try
        {
            for (sol::table const items = interface["items"]; auto const& [key, value] : items)
            {
                sol::table tablified = value;
                auto iid = tablified.get_or<std::string>("id", "");
                if (iid == id)
                {
                    bool fail = false;
                    auto action = tablified.get_or<std::function<void()>>("action", [&fail]()
                    {
                        fail = true;
                    });
                    action();
                    if (!fail)
                    {
                        return "";
                    }
                    else
                        return "item does not have an action, or action is not a function";
                }
            }
        }
        catch(std::exception const& exc)
        {
            return exc.what();
        }
        return "item id not found";
    }
//#####################################################################################################################
}
