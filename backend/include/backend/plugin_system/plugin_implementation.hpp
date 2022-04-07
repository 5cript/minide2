#pragma once

#include <backend/server/frontend_user_session.hpp>

#include <v8wrap/object.hpp>

#include <memory>

namespace Backend::PluginSystem
{
    class Plugin;

    class PluginImplementation
    {
      public:
        void deferredConstruct(
            Plugin* owner,
            v8::Isolate* isolate,
            v8::Local<v8::Value> pluginClass,
            std::weak_ptr<Server::FrontendUserSession> session)
        {
            owner_ = owner;
            pluginClass_.Reset(isolate, pluginClass);
            session_ = std::move(session);
        }

        std::shared_ptr<Server::FrontendUserSession> session()
        {
            return session_.lock();
        }
        std::weak_ptr<Server::FrontendUserSession> weakSession()
        {
            return session_;
        }

        PluginImplementation() = default;
        virtual ~PluginImplementation() = default;
        PluginImplementation(PluginImplementation const&) = delete;
        PluginImplementation(PluginImplementation&&) = default;

        PluginImplementation& operator=(PluginImplementation const&) = delete;
        PluginImplementation& operator=(PluginImplementation&&) = default;

      protected:
        Plugin* owner_;
        v8::Persistent<v8::Value> pluginClass_;

      private:
        std::weak_ptr<Server::FrontendUserSession> session_;
    };
}
