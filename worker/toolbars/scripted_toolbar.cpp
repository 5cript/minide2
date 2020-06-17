#include "scripted_toolbar.hpp"
#include "../scripting_engine/common_state_setup.hpp"

#include <sol/sol.hpp>

using namespace MinIDE::Scripting;

namespace Toolbars
{
//#####################################################################################################################
    struct ScriptedToolbar::Implementation
    {
        sol::state lua;
    };
//#####################################################################################################################
    ScriptedToolbar::ScriptedToolbar(std::string uuid)
        : BasicToolbar{std::move(uuid), ""}
        , impl_{new ScriptedToolbar::Implementation}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void ScriptedToolbar::setupFromScript(MinIDE::Scripting::Script const& script)
    {
        auto& lua = impl_->lua;

        commonStateSetup(lua, true);
        lua["debugging"] = false;

        lua.script(script.script());
    }
//---------------------------------------------------------------------------------------------------------------------
    void ScriptedToolbar::onClick(int id)
    {

    }
//#####################################################################################################################
}
