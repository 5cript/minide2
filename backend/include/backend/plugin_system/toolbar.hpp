#pragma once

#include <backend/server/api/apis.hpp>
#include <backend/filesystem/filesystem.hpp>

namespace PluginSystem
{

class Toolbar
{
public:
    Toolbar(sfs::path const& scriptFile, AllApis const& allApis);

private:
};

}