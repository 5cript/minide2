#pragma once

#include <backend/plugin_system/toolbar_plugin.hpp>
#include <backend/plugin_system/plugin_implementation.hpp>
#include <backend/server/stream/subscriber.hpp>

namespace Backend::PluginSystem
{
    class ToolbarPlugin
        : public PluginImplementation
        , public Server::Stream::Subscriber
    {
      public:
        using PluginImplementation::PluginImplementation;

        void initialize(std::weak_ptr<Server::FrontendUserSession>&& session);
    };
}