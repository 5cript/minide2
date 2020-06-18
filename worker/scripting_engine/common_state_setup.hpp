#pragma once

#include "../filesystem/filesystem.hpp"

#include <sol/sol.hpp>
#include <unordered_map>

namespace MinIDE::Scripting
{
    /**
     *  @param state Lua state
     *  @param enableIo Load lua IO libraries
     *  @param enableOS Load lua OS libraries (DONT, its not useful anyway and a security risk)
     */
    void commonStateSetup(sol::state& state, bool enableIo, bool enableOs = false);

    /**
     *  Ads a search path for lua packages
     */
    void addToPackagePath(sol::state& state, sfs::path const& toAdd);

    void loadLibrariesFromHome(sol::state& state);
}
