#pragma once

#include <backend/server/api/apis.hpp>
#include <backend/filesystem/filesystem.hpp>

namespace PluginSystem
{

class Plugin
{
public:
    constexpr static char const* pluginsDir = "plugins";
    constexpr static char const* pluginDataDir = "plugin_data";
    constexpr static char const* pluginMainFile = "main.js";

    std::string name() const;
    void run() const;

    Plugin(std::string const& pluginName, Api::AllApis const& allApis);
    ~Plugin();
    Plugin(Plugin&&);
    Plugin& operator=(Plugin&&);

private:
    void exposeScriptApi();

private:
    struct Implementation;
    std::unique_ptr <Implementation> impl_;
};

}