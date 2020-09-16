#pragma once

#include "../json.hpp"

#include <string>
#include <optional>

struct RunConfig
{
public:
    struct Debugger
    {
        std::optional <std::string> commandFile;
        std::optional <std::string> initCommandFile;
        std::string debugger; // type
        bool fullyReadSymbols;
        bool returnChildResult;
        std::string path;
        std::string name;
        bool ignoreGdbInit;
        bool ignoreAllGdbInit;
        std::string additionalArguments;
    };

public:
    std::string name;
    std::string type;
    Debugger debugger;
    std::string arguments;
    std::string executeable;

    std::optional <std::string> directory;

    // is a name to one of the configured enviroments
    std::string environment;

    // defaults to true
    bool autostart;
};

void to_json(json& j, RunConfig const& env);
void from_json(json const& j, RunConfig& env);

void to_json(json& j, RunConfig::Debugger const& env);
void from_json(json const& j, RunConfig::Debugger& env);
