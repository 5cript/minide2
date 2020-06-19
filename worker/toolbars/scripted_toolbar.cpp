#include "scripted_toolbar.hpp"
#include "../scripting_engine/common_state_setup.hpp"
#include "../scripting_engine/script.hpp"
#include "../scripting_engine/process.hpp"

#include <sol/sol.hpp>

#include <utility>
#include <type_traits>

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

        Implementation(sfs::path const& toolbarRoot)
            : toolbarRoot{toolbarRoot}
            , engine{new StateCollection}
        {
        }
    };
//#####################################################################################################################
    ScriptedToolbar::ScriptedToolbar(sfs::path const& root)
        : BasicToolbar{""}
        , impl_{new ScriptedToolbar::Implementation(root)}
    {
        initialize();
    }
//---------------------------------------------------------------------------------------------------------------------
    ScriptedToolbar::~ScriptedToolbar() = default;
//---------------------------------------------------------------------------------------------------------------------
    json ScriptedToolbar::getJson() const
    {
        return impl_->jsonRepresentation;
    }
//---------------------------------------------------------------------------------------------------------------------
    void ScriptedToolbar::initialize()
    {
        auto mainScript = MinIDE::Scripting::Script{impl_->toolbarRoot / "main.lua"};

        auto& lua = impl_->engine->lua;

        loadProcessUtility(impl_->engine);

        {
            std::lock_guard <StateCollection::mutex_type> guard{impl_->engine->globalMutex};
            commonStateSetup(lua, true);
            addToPackagePath(lua, impl_->toolbarRoot);

            lua["debugging"] = false;
            lua.script(mainScript.script());
            sol::table items = lua["make_interface"]();

            impl_->jsonRepresentation["items"] = json::array();

            for (auto const& [key, value] : items)
            {
                json jsonItem = json::object();
                sol::table item = value;

                auto transferToJson = [&jsonItem, &item](auto id, auto alternate)
                {
                    auto val = item.get_or<std::decay_t<decltype(alternate)>>(id, alternate);
                    if constexpr(std::is_same_v<std::decay_t<decltype(alternate)>, std::string>)
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
                    transferToJson("special_action", ""s);
                }

                impl_->jsonRepresentation["items"].push_back(jsonItem);
            }
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void ScriptedToolbar::onClick(int id)
    {

    }
//#####################################################################################################################
}
