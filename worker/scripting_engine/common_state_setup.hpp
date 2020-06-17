#pragma once

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
}
