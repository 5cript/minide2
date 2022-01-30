#pragma once

#include <v8wrap/object.hpp>

#include <memory>

class FrontendUserSession;

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

        virtual void initialize(std::weak_ptr<FrontendUserSession>&& session)
        {
            session_ = std::move(session);
        }

    protected:
        std::unique_ptr<v8wrap::Object> pluginClass_;
        std::weak_ptr<FrontendUserSession> session_;
    };
}
