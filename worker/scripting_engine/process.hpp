#pragma once

#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace MinIDE::Scripting
{
    class ScriptedProcess;

    /**
     *  Exposed to lua.
     */
    class LuaProcess
    {
    public:
        LuaProcess(ScriptedProcess* cppProcess);

        /**
         *  @return pair of <has returned?, exit status>
         */
        sol::table tryGetExitStatus();

        /**
         *  @warn BLOCKS!
         */
        int getExitStatus();

        friend struct deleter
        {
            void operator()(LuaProcess* p) const
            {
                destrpy(*p);
            }
        };

        static std::unique_ptr <LuaProcess, deleter> create(ScriptedProcess* cppProcess)
        {
            return std::unique_ptr <LuaProcess, deleter>{new LuaProcess(cppProcess)};
        }

    private:
        static void destroy(LuaProcess& process)

    private:
        ScriptedProcess* cppProcess_;
    };

    using environment_type = std::unordered_map <std::string, std::string>;

    class ScriptedProcess
    {
    public:
        ScriptedProcess(std::string const& id, std::string const& command, std::string const& execDir, environment_type const& env);
        ~ScriptedProcess();

        ScriptedProcess(ScriptedProcess&&) = delete;
        ScriptedProcess& operator=(ScriptedProcess&&) = delete;

        void setStdOutHandler(std::function <void(char const*, std::size_t)> const& cb);
        void setStdErrHandler(std::function <void(char const*, std::size_t)> const& cb);

        std::pair <bool, int> tryGetExitStatus();
        int getExitStatus();

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };

    class ProcessStore
    {
        friend ScriptedProcess;

    public:
        ProcessStore();
        LuaProcess makeProcess(std::string const& command, std::string const& execDir, environment_type const& env);

    private: // friend accesses
        void removeSelf(std::string const& id);
        std::string getFreshId();

    private:
        std::unordered_map <std::string /* id */, ScriptedProcess> processes_;
        long long idCounter_;
    };

    /**
     *  Gives lua the ability to run processes with environment.
     *  These processes are then handled in the returned process store.
     *  You can observe and handle these processes from there and get their I/O which is not exposed to the script (?).
     */
    std::shared_ptr <ProcessStore> loadProcessUtility(sol::state& lua);
}
