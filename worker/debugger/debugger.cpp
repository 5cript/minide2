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
void Debugger::start(std::optional <std::unordered_map <std::string, std::string>> const& env)
{
    DebuggerInterface::UserDefinedArguments args;
    args.debuggerExecuteable = runConfig_.debugger;
    args.commandline = runConfig_.arguments;
    if (env)
        args.environment = env.value();

    auto envDo = [&]()
    {
        debugInterface_  = std::make_shared <DebuggerInterface::Debugger>(args);
    };

    if (!env)
        environmentLockedDo(envDo);
    else
        doWithModifiedPath(envDo, env.value().at("PATH"));
}
//#####################################################################################################################
