#pragma once

#include "../json.hpp"

#include <vector>
#include <string>
#include <map>

namespace SettingParts
{
    class Environment
    {
    public:
        // I provide them for use here.
        // I do explicitly NOT use macros to determine OS, because that might be wrong in whatever context
        // msys2 uses the linux splitter on windows. then there is cygwin, vms, the remote might need the environment of the host for whatever reason.
        // Lets assume we dont exactly know whats needed.
        constexpr static char linuxPathSplit = ':';
        constexpr static char windowsPathSplit = ';';

    public:
        std::vector <std::string> path; // PATH variable, sorted out of variables for its special treatment.
        std::map <std::string, std::string> variables;

        std::map <std::string, std::string> compile(char pathSplit) const;
        std::string mergePath(char pathSplit) const;

        /**
         *  Priority is 'other', if clashing
         *  @param other the other environment to merge with this (other has priority) and return.
         *  @param pathsAreCaseInsensitive Do you want to lowercase match paths in PATH for the merge?
         *                                 On NTFS/windows paths are handled case insensitive, on linux they aren't.
         **/
        Environment merge(Environment const& other, bool pathsAreCaseInsensitive);
    };

    void to_json(json& j, Environment const& env);
    void from_json(json const& j, Environment& env);
}
