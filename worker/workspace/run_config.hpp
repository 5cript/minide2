#pragma once

#include "../filesystem/filesystem.hpp"
#include "../json.hpp"

#include <string>
#include <optional>

class RunConfig
{
public:
    struct Contents
    {
        struct Configuration
        {
            std::string name;
            std::string type;
            std::string debugger;
            std::string arguments;
            std::string executeable;

            std::optional <std::string> directory;

            // is a name to one of the configured enviroments
            std::string environment;

            // defaults to true
            bool autostart;
        };
        std::vector <Configuration> configs;
    };

public:
    RunConfig(sfs::path const& root);

    bool load();
    std::string raw() const;

    std::optional <Contents::Configuration> findProfile(std::string const& name);

private:
    void transferExistingItems(json const& j);

private:
    sfs::path root_;
    std::string raw_;
    Contents content_;
};
