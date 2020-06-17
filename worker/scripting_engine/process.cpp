#include "process.hpp"

#include <tiny-process-library/process.hpp>

namespace MinIDE::Scripting
{
//#####################################################################################################################
    LuaProcess::LuaProcess(ScriptedProcess* cppProcess)
        : cppProcess_{cppProcess}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    sol::table LuaProcess::tryGetExitStatus()
    {
        auto exitStatus = cppProcess_->tryGetExitStatus();
        sol::table status;
        status["hasEnded"] = exitStatus.first;
        status["statusCode"] = exitStatus.second;
        return status;
    }
//---------------------------------------------------------------------------------------------------------------------
    void LuaProcess::destroy(LuaProcess& process)
    {
        std::cout << "~LuaProcess from lua";
        process.~LuaProcess();
    }
//---------------------------------------------------------------------------------------------------------------------
    int LuaProcess::getExitStatus()
    {
        std::cout << "get exit status\n";
        auto status = cppProcess_->getExitStatus();
        return status;
    }
//#####################################################################################################################
    struct ScriptedProcess::Implementation
    {
        TinyProcessLib::Process process;
        std::string id;
        std::function <void(char const* c, std::size_t amount)> onStdOut;
        std::function <void(char const* c, std::size_t amount)> onStdErr;

        Implementation(ScriptedProcess* owner, std::string const& id, std::string const& command, std::string const& execDir, environment_type const& env)
            : process {
                command,
                execDir,
                env,
                [this](char const* c, auto amount)
                {
                    if (onStdOut)
                        onStdOut(c, amount);
                    std::cout << "[STDOUT]" << std::string{c, amount} << "\n";
                },
                [this](char const* c, auto amount)
                {
                    if (onStdErr)
                        onStdErr(c, amount);
                    std::cout << "[STDERR]"  << std::string{c, amount} << "\n";
                },
                false /* dont open stdin for now, redecide later if needed */
            }
            , id{id}
            , onStdOut{}
            , onStdErr{}
        {
        }
    };
//#####################################################################################################################
    ScriptedProcess::ScriptedProcess
    (
        std::string const& id,
        std::string const& command,
        std::string const& execDir,
        environment_type const& env
    )
        : impl_{new ScriptedProcess::Implementation(this, id, command, execDir, env)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void ScriptedProcess::setStdOutHandler(std::function <void(char const*, std::size_t)> const& cb)
    {
        impl_->onStdOut = cb;
    }
//---------------------------------------------------------------------------------------------------------------------
    ScriptedProcess::~ScriptedProcess()
    {
        auto status = tryGetExitStatus();

        // kill process if it isnt dead yet. so it doenst become a zombie
        if (!status.first)
            impl_->process.kill(true);
    }
//---------------------------------------------------------------------------------------------------------------------
    std::pair <bool, int> ScriptedProcess::tryGetExitStatus()
    {
        int exitStatus = 0;
        auto processReturned = impl_->process.try_get_exit_status(exitStatus);

        return {processReturned, exitStatus};
    }
//---------------------------------------------------------------------------------------------------------------------
    int ScriptedProcess::getExitStatus()
    {
        return impl_->process.get_exit_status();
    }
//---------------------------------------------------------------------------------------------------------------------
    void ScriptedProcess::setStdErrHandler(std::function <void(char const*, std::size_t)> const& cb)
    {
        impl_->onStdErr = cb;
    }
//#####################################################################################################################
    ProcessStore::ProcessStore()
        : processes_{}
        , idCounter_{0}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    LuaProcess ProcessStore::makeProcess(std::string const& command, std::string const& execDir, environment_type const& env)
    {
        auto id = getFreshId();
        auto emplacementResult = processes_.emplace(
            std::piecewise_construct,
            std::make_tuple(id),
            std::make_tuple(id, command, execDir, env)
        );
        if (!emplacementResult.second)
            throw std::runtime_error("could not create process");
        return {&emplacementResult.first->second};
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string ProcessStore::getFreshId()
    {
        return std::to_string(idCounter_++);
    }
//#####################################################################################################################
//---------------------------------------------------------------------------------------------------------------------
    std::shared_ptr <ProcessStore> loadProcessUtility(sol::state& lua)
    {
        auto store = std::make_shared<ProcessStore>();

        sol::usertype<LuaProcess> luaProcessType = lua.new_usertype<LuaProcess>
        (
            "LuaProcess",
            sol::no_constructor,
            "try_get_exit_status", [](LuaProcess& p){return p.tryGetExitStatus();},
            "get_exit_status", [](LuaProcess& p){std::cout << "ges\n"; return p.getExitStatus();}
        );

        lua.set_function
        (
            "executeProcess",
            [store](std::string const& command, std::string const& execDir, sol::table const& environmentTable)
                -> LuaProcess
            {
                environment_type environment;

                for (auto const& [key, value] : environmentTable)
                    environment.emplace(std::make_pair(key.as<std::string>(), value.as<std::string>()));

                return store->makeProcess(command, execDir, environment);
            }
        );

        return store;
    }
//#####################################################################################################################
}
