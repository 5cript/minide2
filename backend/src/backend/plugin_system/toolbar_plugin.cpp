#include <backend/plugin_system/toolbar_plugin.hpp>
#include <backend/plugin_system/api/console.hpp>

#include <v8wrap/array.hpp>

namespace PluginSystem
{
//#####################################################################################################################
    ToolbarPlugin::ToolbarPlugin(std::unique_ptr<v8wrap::Object>&& pluginClass)
        : PluginImplementation{std::move(pluginClass)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void ToolbarPlugin::initialize()
    {
        v8::HandleScope scope{pluginClass_->isolate()};
        v8wrap::Array array{pluginClass_->context(), pluginClass_->call("initialize")};
        PluginApi::Console::print(pluginClass_->context(), array);
    }
//#####################################################################################################################
}