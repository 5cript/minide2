#include "process.hpp"
#include "../environment_lock.hpp"

#include <tiny-process-library/process.hpp>
#include <iostream>
#include <mutex>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>

#ifdef _WIN32
#   include <windows.h>
#endif

using namespace std::string_literals;
using namespace std::chrono_literals;

namespace MinIDE::Scripting
{
//#####################################################################################################################
    struct LuaProcess::Implementation
    {
        std::weak_ptr <StateCollection> weakStateRef;
        std::unique_ptr <TinyProcessLib::Process> process;
        sol::protected_function onStdOut;
        sol::protected_function onStdErr;
        sol::protected_function onExit;
        std::thread lifetimeControl;
        std::atomic <bool> kill;
        int exitStatus;
        std::atomic <bool> ended;

        bool runCheckAndJoin();

        Implementation(std::weak_ptr <StateCollection>&& stateRef)
            : weakStateRef{std::move(stateRef)}
            , process{}
            , onStdOut{}
            , onStdErr{}
            , lifetimeControl{}
            , kill{false}
            , exitStatus{0}
            , ended{false}
        {
        }
    };
//---------------------------------------------------------------------------------------------------------------------
    bool LuaProcess::Implementation::runCheckAndJoin()
    {
        auto const joinable = lifetimeControl.joinable();
        auto const hasEnded = ended.load();
        if (joinable && hasEnded)
        {
            lifetimeControl.join();
            return true;
        }
        else if (joinable && !hasEnded)
            return false;
        else /*if (!joinable)*/
            return true;
    }
//#####################################################################################################################
    LuaProcess::LuaProcess(std::weak_ptr <StateCollection> weakStateRef)
        : impl_{new LuaProcess::Implementation(std::move(weakStateRef))}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    LuaProcess::~LuaProcess()
    {
        impl_->kill.store(true);
        if (impl_->lifetimeControl.joinable())
            impl_->lifetimeControl.join();
    }
//---------------------------------------------------------------------------------------------------------------------
    int LuaProcess::executeShort
    (
        std::string const& command,
        std::string const& execDir,
        std::map <std::string, std::string> const& environment
    )
    {
        if (!impl_->runCheckAndJoin())
            return -1;

        int error = 0;
        std::unordered_map <std::string, std::string> env;
        for (auto const& [key, value] : environment)
        {
            auto modKey = key;
#ifdef _WIN32
            std::transform(std::begin(modKey), std::end(modKey), std::begin(modKey), [](auto c)
            {
                return std::toupper(c);
            });
#endif
            auto pair = std::make_pair(modKey, value);
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

        if (error == 0)
        {
            impl_->lifetimeControl = std::thread{[this]()
            {
                auto whenExit = [this](int status)
                {
                    impl_->ended.store(true);
                    if (impl_->onExit)
                    {
                        auto ptr = impl_->weakStateRef.lock();
                        if (ptr)
                        {
                            std::lock_guard <StateCollection::mutex_type> guard{ptr->globalMutex};
                            impl_->onExit(status);
                        }
                    }
                };

                impl_->ended.store(false);
                for (;impl_->kill.load() == false;)
                {
                    int status = 0;
                    if (impl_->process->try_get_exit_status(status))
                    {
                        impl_->exitStatus = status;
                        whenExit(status);
                        return;
                    }

                    std::this_thread::yield();
                    std::this_thread::sleep_for(100ms);
                }
                int status = 0;
                if (!impl_->process->try_get_exit_status(status))
                {
                    impl_->process->kill(true);
                    auto status = impl_->process->get_exit_status();
                    whenExit(status);
                }
                else
                    whenExit(status);
            }};
        }

        return error;
    }
//---------------------------------------------------------------------------------------------------------------------
    int LuaProcess::execute
    (
        std::string const& command,
        std::string const& execDir,
        std::map <std::string, std::string> const& environment,
        sol::protected_function const& onStdOut,
        sol::protected_function const& onStdErr,
        sol::protected_function const& onExit
    )
    {
        if (!impl_->runCheckAndJoin())
            return -1;

        impl_->onStdOut = onStdOut;
        impl_->onStdErr = onStdErr;
        impl_->onExit = onExit;
        return executeShort(command, execDir, environment);
    }
//---------------------------------------------------------------------------------------------------------------------
    std::optional <int> LuaProcess::tryGetExitStatus()
    {
        if (!impl_->runCheckAndJoin())
            return std::nullopt;

        return impl_->exitStatus;
    }
//---------------------------------------------------------------------------------------------------------------------
    void LuaProcess::kill(bool force)
    {
        if (impl_->lifetimeControl.joinable())
        {
            impl_->kill.store(true);
            impl_->lifetimeControl.join();
            impl_->process.reset(nullptr);
        }
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
            "kill", &LuaProcess::kill,
            "execute", sol::overload
            (
                &LuaProcess::execute,
                &LuaProcess::executeShort
            )
        );
    }
//#####################################################################################################################
}
