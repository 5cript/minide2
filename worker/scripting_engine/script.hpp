#pragma once

#include "../filesystem/filesystem.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace MinIDE::Scripting
{
    /**
     *  Contains a Lua Script for execution.
     */
    class Script
    {
    public:
        Script(sfs::path const& scriptFile);
        Script(std::string string);
        Script();

        std::string_view viewScript();
        std::string const& script() const;

    private:
        std::string script_;
    };
}
