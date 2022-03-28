#pragma once

#include <backend/server/api/apis.hpp>
#include <backend/filesystem/filesystem.hpp>

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
        void run() const;

        Plugin(std::string const& pluginName, Server::Api::AllApis const& allApis);
        ~Plugin();
        Plugin(Plugin&&);
        Plugin& operator=(Plugin&&);

        void initialize(std::weak_ptr<Server::FrontendUserSession> session) const;

      private:
        void exposeGlobals() const;
        void extractExports() const;

      private:
        struct Implementation;
        std::unique_ptr<Implementation> impl_;
    };

}