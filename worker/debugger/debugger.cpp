#include "debugger.hpp"
#include "../environment_lock.hpp"

#include <debugger-interface/debugger.hpp>
#include <memory>

//#####################################################################################################################
Debugger::Debugger
(
    RunConfig::Contents::Configuration const& usedConfig,
    std::string instanceId
)
    : runConfig_{usedConfig}
    , instanceId_{std::move(instanceId)}
    , debugInterface_{}
{
}
//---------------------------------------------------------------------------------------------------------------------
Debugger::~Debugger() = default;
//---------------------------------------------------------------------------------------------------------------------
std::string_view Debugger::runConfigName() const
{
    return runConfig_.name;
}
//---------------------------------------------------------------------------------------------------------------------
RunConfig::Contents::Configuration Debugger::runConfig() const
{
    return runConfig_;
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onRawData(std::string const& raw)
{
    std::cout << raw << "\n";
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onLogStream(std::string const& message)
{
    std::cout << message << "\n";
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::onConsoleStream(std::string const& message)
{
    std::cout << message << "\n";
}
//---------------------------------------------------------------------------------------------------------------------
void Debugger::start(std::optional <std::unordered_map <std::string, std::string>> const& env)
{
    DebuggerInterface::UserDefinedArguments args;
    args.debuggerExecuteable = runConfig_.debugger;
    args.commandline = runConfig_.arguments;
    if (env)
        args.environment = env.value();
    args.program = runConfig_.executeable;
    args.directory = runConfig_.directory;


    auto envDo = [&]()
    {
        debugInterface_  = std::make_shared <DebuggerInterface::Debugger>(args);
        debugInterface_->start();
    };

    if (!env)
        environmentLockedDo(envDo);
    else
        doWithModifiedPath(envDo, env.value().at("PATH"));
}
//#####################################################################################################################
