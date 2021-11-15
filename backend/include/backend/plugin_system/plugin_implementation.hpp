#pragma once

#include <v8wrap/object.hpp>

#include <memory>

namespace PluginSystem
{
    class PluginImplementation
    {
    public:
        PluginImplementation(std::unique_ptr<v8wrap::Object>&& pluginClass)
            : pluginClass_{std::move(pluginClass)}
        {}
        virtual ~PluginImplementation() = default;
        PluginImplementation(PluginImplementation const&) = delete;
        PluginImplementation(PluginImplementation&&) = default;
        
        PluginImplementation& operator=(PluginImplementation const&) = delete;
        PluginImplementation& operator=(PluginImplementation&&) = default;

        virtual void initialize() = 0;

    protected:
        std::unique_ptr<v8wrap::Object> pluginClass_;
    };
}
