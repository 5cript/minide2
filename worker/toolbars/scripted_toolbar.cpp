#include "scripted_toolbar.hpp"
#include "../scripting_engine/common_state_setup.hpp"
#include "../scripting_engine/script.hpp"
#include "../scripting_engine/process.hpp"

#include <sol/sol.hpp>

using namespace MinIDE::Scripting;

namespace Toolbars
{
//#####################################################################################################################
    struct ScriptedToolbar::Implementation
    {
        sfs::path toolbarRoot;
        std::shared_ptr <StateCollection> engine;

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
            lua["make_interface"]();
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void ScriptedToolbar::onClick(int id)
    {

    }
//#####################################################################################################################
}
