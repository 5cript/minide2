#pragma once

#include "state.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <map>

namespace MinIDE::Scripting
{
    /**
     *  Exposed to lua.
     */
    class LuaProcess
    {
    public:
        LuaProcess(std::weak_ptr <StateCollection> weakStateRef);
        ~LuaProcess();

        int executeShort
        (
            std::string const& command,
            std::string const& execDir,
            std::unordered_map <std::string, std::string> const& environment
        );

        int execute
        (
            std::string const& command,
            std::string const& execDir,
            std::unordered_map <std::string, std::string> const& environment,
            sol::protected_function const& onStdOut,
            sol::protected_function const& onStdErr,
            sol::protected_function const& onExit
        );

        /**
         *  @return pair of <has returned?, exit status>
         */
        std::optional <int> tryGetExitStatus();

        void kill(bool force);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };

    /**
     *  Gives lua the ability to run processes with environment.
     *  These processes are then handled in the returned process store.
     *  You can observe and handle these processes from there and get their I/O which is not exposed to the script (?).
     */
    void loadProcessUtility(std::weak_ptr <StateCollection> lua);
}
