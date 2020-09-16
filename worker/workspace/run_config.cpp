#include "run_config.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

namespace
{
    template <typename T>
    struct UnpackOptional
    {
        using type = T;
    };

    template <typename U>
    struct UnpackOptional <std::optional <U>>
    {
        using type = U;
    };
}

//#####################################################################################################################
void to_json(json& j, RunConfig const& env)
{
    j["name"] = env.name;
    j["type"] = env.type;
    j["debugger"] = env.debugger;
    j["arguments"] = env.arguments;
    j["executeable"] = env.executeable;
    if (env.directory)
        j["directory"] = env.directory.value();
    j["environment"] = env.environment;
    j["autostart"] = env.autostart;
}
//---------------------------------------------------------------------------------------------------------------------
void from_json(json const& j, RunConfig& env)
{
    env.name = j["name"].get<std::string>();
    env.type = j["type"].get<std::string>();
    env.debugger = j["debugger"].get<RunConfig::Debugger>();
    env.arguments = j["arguments"].get<std::string>();
    env.executeable = j["executeable"].get<std::string>();
    if (j.contains("directory"))
        env.directory  = j["directory"].get<std::string>();
    else
        env.directory = std::nullopt;
    env.environment = j["environment"].get<std::string>();
    env.autostart = j["autostart"].get<bool>();
}
//---------------------------------------------------------------------------------------------------------------------
void to_json(json& j, RunConfig::Debugger const& env)
{
    if (env.commandFile)
        j["commandFile"] = env.commandFile.value();
    if (env.initCommandFile)
        j["initCommandFile"] = env.initCommandFile.value();
    j["debugger"] = env.debugger;
    j["fullyReadSymbols"] = env.fullyReadSymbols;
    j["returnChildResult"] = env.returnChildResult;
    j["path"] = env.path;
    j["name"] = env.name;
    j["ignoreGdbInit"] = env.ignoreGdbInit;
    j["ignoreAllGdbInit"] = env.ignoreAllGdbInit;
    j["additionCommandlineArguments"] = env.additionalArguments;
}
//---------------------------------------------------------------------------------------------------------------------
void from_json(json const& j, RunConfig::Debugger& env)
{
    if (j.contains("commandFile") && !j["commandFile"].is_null())
        env.commandFile  = j["commandFile"].get<std::string>();
    if (j.contains("initCommandFile") && !j["initCommandFile"].is_null())
        env.initCommandFile  = j["initCommandFile"].get<std::string>();
    env.debugger = j["debugger"].get<std::string>();

    if (j.contains("fullyReadSymbols"))
        env.fullyReadSymbols = j["fullyReadSymbols"].get<bool>();
    else
        env.fullyReadSymbols = true;

    if (j.contains("returnChildResult"))
        env.returnChildResult = j["returnChildResult"].get<bool>();
    else
        env.returnChildResult = true;

    env.path = j["path"].get<std::string>();
    env.name = j["name"].get<std::string>();

    if (j.contains("ignoreGdbInit"))
        env.ignoreGdbInit = j["ignoreGdbInit"].get<bool>();
    else
        env.ignoreGdbInit = false;
    if (j.contains("ignoreAllGdbInit"))
        env.ignoreAllGdbInit = j["ignoreAllGdbInit"].get<bool>();
    else
        env.ignoreAllGdbInit = false;
    if (j.contains("additionCommandlineArguments"))
        env.additionalArguments = j["additionCommandlineArguments"].get<std::string>();
    else
        env.additionalArguments = "";
}
//#####################################################################################################################
