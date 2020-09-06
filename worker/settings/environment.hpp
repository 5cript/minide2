#pragma once

#include "../json.hpp"

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

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
        std::unordered_map <std::string, std::string> variables;

        /// A list (map, because integer indicates order because guarantees for arrays might be tricky to hold)
        /// of other environments to inherit from.
        /// Avoid usage, unless you really think its right for your case.
        std::map <std::string, std::string> inherits;

        std::unordered_map <std::string, std::string> compile(char pathSplit) const;
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
