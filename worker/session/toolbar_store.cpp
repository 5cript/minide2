#include "toolbar_store.hpp"

//#####################################################################################################################
void ToolbarStore::reset()
{
    scriptedToolbars.clear();
}
//---------------------------------------------------------------------------------------------------------------------
Toolbars::ScriptedToolbar* ToolbarStore::toolbarById(std::string const& id)
{
    for (auto const& toolbar : scriptedToolbars)
    {
        if (toolbar->id() == id)
            return toolbar.get();
    }
    return nullptr;
}
//#####################################################################################################################
