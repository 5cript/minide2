#pragma once

#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace MinIDE::Scripting
{
    /**
     *  Exposed to lua.
     */
    class LuaProcess
    {
    public:
        LuaProcess();

        int execute
        (
            std::string const& command,
            std::string const& execDir,
            sol::table environment
        );

        /**
         *  @return pair of <has returned?, exit status>
         */
        sol::table tryGetExitStatus();

        /**
         *  @warn BLOCKS!
         */
        int getExitStatus();

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };

    /**
     *  Gives lua the ability to run processes with environment.
     *  These processes are then handled in the returned process store.
     *  You can observe and handle these processes from there and get their I/O which is not exposed to the script (?).
     */
    void loadProcessUtility(sol::state& lua);
}
