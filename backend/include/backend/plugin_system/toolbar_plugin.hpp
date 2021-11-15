#pragma once

#include <backend/plugin_system/toolbar_plugin.hpp>
#include <backend/plugin_system/plugin_implementation.hpp>

namespace PluginSystem
{
    class ToolbarPlugin : public PluginImplementation
    {
    public:
        ToolbarPlugin(std::unique_ptr<v8wrap::Object>&& pluginClass);
        void initialize();
    };
}