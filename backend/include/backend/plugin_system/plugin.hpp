#pragma once

#include <backend/server/api/apis.hpp>
#include <backend/filesystem/filesystem.hpp>

#include <v8.h>

class FrontendUserSession;

namespace Backend::PluginSystem
{

    class Plugin
    {
      public:
        constexpr static char const* pluginsDir = "plugins";
        constexpr static char const* pluginDataDir = "plugin_data";
        constexpr static char const* pluginMainFile = "main.js";

        std::string name() const;
        void run(std::weak_ptr<Server::FrontendUserSession> session);

        void performAsyncScriptAction(std::function<void(v8::Local<v8::Context> context)> const& action);

        Plugin(std::string const& pluginName, Server::Api::AllApis const& allApis);
        ~Plugin();
        Plugin(Plugin&&);
        Plugin& operator=(Plugin&&);

      private:
        void exposeGlobals() const;
        void extractExports(std::weak_ptr<Server::FrontendUserSession>&& session);

      private:
        struct Implementation;
        std::unique_ptr<Implementation> impl_;
    };

}