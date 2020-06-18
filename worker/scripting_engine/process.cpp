#include "process.hpp"
#include "../environment_lock.hpp"

#include <tiny-process-library/process.hpp>
#include <iostream>
#include <mutex>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#   include <windows.h>
#endif

using namespace std::string_literals;

namespace MinIDE::Scripting
{
//#####################################################################################################################
    struct LuaProcess::Implementation
    {
        std::weak_ptr <StateCollection> weakStateRef;
        std::unique_ptr <TinyProcessLib::Process> process;
        sol::function onStdOut;
        sol::function onStdErr;

        Implementation(std::weak_ptr <StateCollection>&& stateRef)
            : weakStateRef{std::move(stateRef)}
            , process{}
            , onStdOut{}
            , onStdErr{}
        {
        }
    };
//#####################################################################################################################
    LuaProcess::LuaProcess(std::weak_ptr <StateCollection> weakStateRef)
        : impl_{new LuaProcess::Implementation(std::move(weakStateRef))}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    LuaProcess::~LuaProcess() = default;
//---------------------------------------------------------------------------------------------------------------------
    int LuaProcess::executeShort
    (
        std::string const& command,
        std::string const& execDir,
        sol::table environment
    )
    {
        int error = 0;
        std::unordered_map <std::string, std::string> env;
        for (auto const& [key, value] : environment)
        {
            auto modKey = key.as<std::string>();
#ifdef _WIN32
            std::transform(std::begin(modKey), std::end(modKey), std::begin(modKey), [](auto c)
            {
                return std::toupper(c);
            });
#endif
            auto pair = std::make_pair(modKey, value.as<std::string>());
            env.emplace(pair);
        }

        auto runProcess = [&, this]()
        {
            impl_->process = std::make_unique <TinyProcessLib::Process>
            (
                command,
                execDir,
                env,
                [this](char const* c, auto amount)
                {
                    auto strongRef = impl_->weakStateRef.lock();
                    if (!strongRef)
                        return;
                    std::lock_guard <StateCollection::mutex_type> {strongRef->globalMutex};

                    if (impl_->onStdOut)
                        impl_->onStdOut(std::string{c, amount});
                    //std::cout << "[STDOUT]" << std::string{c, amount} << "\n";
                },
                [this](char const* c, auto amount)
                {
                    auto strongRef = impl_->weakStateRef.lock();
                    if (!strongRef)
                        return;
                    std::lock_guard <StateCollection::mutex_type> {strongRef->globalMutex};

                    if (impl_->onStdErr)
                        impl_->onStdErr(std::string{c, amount});
                    //std::cout << "[STDERR]"  << std::string{c, amount} << "\n";
                },
                false /* dont open stdin for now, redecide later if needed */
            );

#ifdef _WIN32
            error = GetLastError();
#else
            error = errno;
#endif
        };

        // execution lock scope
        auto pIter = env.find("PATH");
        if (pIter != std::end(env))
            doWithModifiedPath(runProcess, pIter->second);
        else
            environmentLockedDo(runProcess);

        return error;
    }
//---------------------------------------------------------------------------------------------------------------------
    int LuaProcess::execute
    (
        std::string const& command,
        std::string const& execDir,
        sol::table environment,
        sol::function const& onStdOut,
        sol::function const& onStdErr
    )
    {
        impl_->onStdOut = onStdOut;
        impl_->onStdErr = onStdErr;
        return executeShort(command, execDir, environment);
    }
//---------------------------------------------------------------------------------------------------------------------
    sol::table LuaProcess::tryGetExitStatus()
    {
        int status = 0;
        sol::table tab;
        tab["hasEnded"] = impl_->process->try_get_exit_status(status);
        tab["status"] = status;
        return tab;
    }
//---------------------------------------------------------------------------------------------------------------------
    int LuaProcess::getExitStatus()
    {
        return impl_->process->get_exit_status();
    }
//#####################################################################################################################
    void loadProcessUtility(std::weak_ptr <StateCollection> state)
    {
        auto strongRef = state.lock();
        if (!strongRef)
            return;
        std::lock_guard <StateCollection::mutex_type> {strongRef->globalMutex};

        auto usertype = strongRef->lua.new_usertype<LuaProcess>
        (
            "Process",
            "new", sol::initializers
            (
                [state](LuaProcess& p) -> void
                {
                    new (&p) LuaProcess(state);
                }
            ),
            "try_get_exit_status", &LuaProcess::tryGetExitStatus,
            "get_exit_status", &LuaProcess::getExitStatus,
            "execute", sol::overload
            (
                &LuaProcess::execute,
                &LuaProcess::executeShort
            )
        );
    }
//#####################################################################################################################
}
