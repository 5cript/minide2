#include "scripted_toolbar.hpp"
#include "../scripting_engine/common_state_setup.hpp"
#include "../scripting_engine/script.hpp"
#include "../scripting_engine/process.hpp"
#include "../scripting_engine/project_control.hpp"
#include "../scripting_engine/streamer_access.hpp"
#include "../scripting_engine/settings_provider.hpp"
#include "../session/session_obtainer.hpp"
#include "../routers/streamer.hpp"

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

        std::pair <std::string, sol::table> getItem(std::string const& itemId);
    };
//---------------------------------------------------------------------------------------------------------------------
    std::pair <std::string, sol::table> ScriptedToolbar::Implementation::getItem(std::string const& itemId)
    {
        std::lock_guard <StateCollection::mutex_type> guard{engine->globalMutex};
        auto& lua = engine->lua;

        if (!loaded)
            return {"not loaded (how did this happen?) "s + __FILE__ + ":" + std::to_string(__LINE__), {}};

        if (!lua["get_toolbar"].valid())
            return {"missing get_toolbar function", {}};

        sol::table interface = lua["get_toolbar"]();

        if (!interface["items"].valid())
            return {"toolbar is missing items", {}};

        sol::table items = interface["items"];

        try
        {
            for (sol::table const items = interface["items"]; auto const& [key, value] : items)
            {
                sol::table tablified = value;
                auto id = tablified.get_or<std::string>("id", "");
                if (id != itemId)
                    continue;

                return {""s, tablified};
            }
        }
        catch(std::exception const& exc)
        {
            return {std::string{exc.what()}, {}};
        }
        return {"item id not found", {}};
    }
//#####################################################################################################################
    ScriptedToolbar::ScriptedToolbar
    (
        sfs::path const& root,
        SessionObtainer const& obtainer,
        Routers::DataStreamer* streamer,
        Routers::SettingsProvider* settingsProv
    )
        : BasicToolbar{""}
        , impl_{new ScriptedToolbar::Implementation(root)}
    {
        initialize(obtainer, streamer, settingsProv);
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
    void ScriptedToolbar::initialize
    (
        SessionObtainer const& obtainer,
        Routers::DataStreamer* streamer,
        Routers::SettingsProvider* settingsProv
    )
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
            loadStreamerAccess(impl_->engine, obtainer, streamer);
            loadSettingsProvider(impl_->engine, obtainer, streamer, settingsProv);

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

                // Converts lua table entries to JSON if they exist
                auto transfer = [](auto& j, sol::table& luaItem, auto const& id, auto alternate)
                {
                    using alterType = std::decay_t<decltype(alternate)>;
                    auto val = luaItem.get_or<alterType>(id, alternate);
                    if constexpr
                    (
                        std::is_same_v<std::decay_t<decltype(alternate)>, std::string> ||
                        std::is_same_v<std::decay_t<decltype(alternate)>, std::vector <std::string>>
                    )
                    {
                        if (val.empty())
                            return;
                    }
                    j[id] = val;
                };

                auto transferBasics = [&jsonItem, &item, &transfer](auto const& id, auto const& alternate)
                {
                    transfer(jsonItem, item, id, alternate);
                };

                // No Type No Deal
                if (!item["type"].valid())
                    continue;

                // Type
                jsonItem["type"] = item.get<std::string>("type");

                // Id
                transferBasics("id", "missing_id_"s + key.as<std::string>());

                // Item Type dependent transferals
                std::string type = jsonItem["type"];
                if (type == "IconButton")
                {
                    transferBasics("pngbase64", ""s);
                    transferBasics("special_actions", std::vector <std::string>{});
                }
                else if (type == "Menu")
                {
                    if (item["entries"].valid())
                    {
                        jsonItem["entries"] = json::array();
                        for (sol::table const entries = item["entries"]; auto const& [key, value] : entries)
                        {
                            json jentry = json::object();
                            sol::table entry = value;

                            transfer(jentry, entry, "label", ""s);
                            transfer(jentry, entry, "is_splitter", false);
                            transfer(jentry, entry, "pngbase64", ""s);
                            transfer(jentry, entry, "special_actions", std::vector <std::string>{});


                            jsonItem["entries"].push_back(jentry);
                        }
                    }
                }

                impl_->jsonRepresentation["items"].push_back(jsonItem);
            }

            impl_->loaded = true;
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string ScriptedToolbar::clickAction(std::string const& id)
    {
        auto p = impl_->getItem(id);
        if (!p.first.empty())
            return p.first;

        std::lock_guard <StateCollection::mutex_type> guard{impl_->engine->globalMutex};

        auto& tablified = p.second;

        bool fail = false;
        auto action = tablified.get_or<std::function<void()>>("action", [&fail]()
        {
            fail = true;
        });
        action();
        if (!fail)
            return "";
        else
            return "item does not have an action, or action is not a function";
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string ScriptedToolbar::menuAction(std::string const& itemId, std::string const& menuEntryLabel)
    {
        auto p = impl_->getItem(itemId);
        if (!p.first.empty())
            return p.first;

        std::lock_guard <StateCollection::mutex_type> guard{impl_->engine->globalMutex};
        auto& tablified = p.second;

        if (!tablified["entries"].valid())
            return "no entries in menu";

        for (sol::table const entries = tablified["entries"]; auto const& [ek, entry] : entries)
        {
            sol::table entryTable = entry;
            auto label = entryTable.get_or<std::string>("label", "");
            if (label != menuEntryLabel)
                continue;

            if (!entryTable["action"].valid())
                return "item.entry does not have an action";

            auto action = entryTable.get_or<std::function<void()>>("action", [](){});
            action();
            return "";
        }
        return "menu was found, but not the label of the menu entry";
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string ScriptedToolbar::loadCombobox(std::string const& itemId)
    {
        auto p = impl_->getItem(itemId);
        if (!p.first.empty())
            return p.first;

        std::lock_guard <StateCollection::mutex_type> guard{impl_->engine->globalMutex};
        auto& item = p.second;

        bool fail = false;
        auto load = item.get_or<std::function<void()>>("load", [&fail]()
        {
            fail = true;
        });
        load();
        if (!fail)
            return "";
        else
            return "item does not have a load function, or member is not a function";
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string ScriptedToolbar::comboboxSelect(std::string const& itemId, std::string const& selected)
    {
        std::lock_guard <StateCollection::mutex_type> guard{impl_->engine->globalMutex};

        auto& lua = impl_->engine->lua;

        if (!impl_->loaded)
            return "not loaded (how did this happen?) "s + __FILE__ + ":" + std::to_string(__LINE__);

        if (!lua["combox_select"].valid())
            return "missing combox_select function";

        bool fail = false;
        auto select = lua.get_or<std::function<void(std::string const&, std::string const&)>>
        (
            "combox_select",
            [&fail](auto const&, auto const&){fail = true;}
        );
        select(itemId, selected);
        if (fail)
            return "script interface misses the combox_select function";
        return "";
    }
//#####################################################################################################################
}
