#include "process.hpp"

#include <tiny-process-library/process.hpp>
#include <iostream>
#include <mutex>

#ifdef _WIN32
#   include <windows.h>
#endif

using namespace std::string_literals;

static std::mutex globalProcessExecutionLock;

namespace MinIDE::Scripting
{
//#####################################################################################################################
    struct LuaProcess::Implementation
    {
        std::unique_ptr <TinyProcessLib::Process> process;
        std::function <void(char const* c, std::size_t amount)> onStdOut;
        std::function <void(char const* c, std::size_t amount)> onStdErr;
    };
//#####################################################################################################################
    LuaProcess::LuaProcess()
        : impl_{new LuaProcess::Implementation()}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    int LuaProcess::execute
    (
        std::string const& command,
        std::string const& execDir,
        sol::table environment
    )
    {
        std::unordered_map <std::string, std::string> env;
        for (auto const& [key, value] : environment)
        {
            auto pair = std::make_pair(key.as<std::string>(), value.as<std::string>());
            env.emplace(pair);
        }

        // execution lock scope
        {
            std::lock_guard <std::mutex> {globalProcessExecutionLock};

            auto setPath = [](std::string const& to)
            {
#ifdef _WIN32
                std::string p = "PATH="s + to;
                _putenv(p.c_str());
#else
                setenv("PATH", to.c_str(), 1);
#endif
            };

            std::string prevPath{getenv("PATH")};
            auto pathIter = env.find("PATH");
            if (pathIter != std::end(env))
                setPath(pathIter->second);

            impl_->process = std::make_unique <TinyProcessLib::Process>
            (
                command,
                execDir,
                env,
                [this](char const* c, auto amount)
                {
                    if (impl_->onStdOut)
                        impl_->onStdOut(c, amount);
                    //std::cout << "[STDOUT]" << std::string{c, amount} << "\n";
                },
                [this](char const* c, auto amount)
                {
                    if (impl_->onStdErr)
                        impl_->onStdErr(c, amount);
                    //std::cout << "[STDERR]"  << std::string{c, amount} << "\n";
                },
                false /* dont open stdin for now, redecide later if needed */
            );

            if (pathIter != std::end(env))
                setPath(prevPath);
        }

#ifdef _WIN32
        return GetLastError();
#else
        return errno;
#endif
    }
//---------------------------------------------------------------------------------------------------------------------
    sol::table LuaProcess::tryGetExitStatus()
    {
        return sol::table{};
    }
//---------------------------------------------------------------------------------------------------------------------
    int LuaProcess::getExitStatus()
    {
        return 0;
    }
//#####################################################################################################################
//---------------------------------------------------------------------------------------------------------------------
    void loadProcessUtility(sol::state& lua)
    {
        auto usertype = lua.new_usertype<LuaProcess>
        (
            "Process",
            sol::constructors<LuaProcess()>(),
            "try_get_exit_status", &LuaProcess::tryGetExitStatus,
            "get_exit_status", &LuaProcess::getExitStatus,
            "execute", &LuaProcess::execute
        );
    }
//#####################################################################################################################
}
