#pragma once

#include <backend/plugin_system/toolbar_plugin.hpp>
#include <backend/plugin_system/plugin_implementation.hpp>

namespace PluginSystem
{
    class ToolbarPlugin : public PluginImplementation
    {
      public:
        using PluginImplementation::PluginImplementation;

        void initialize(std::weak_ptr<FrontendUserSession>&& session);
    };
}