#pragma once

#include <backend/settings/environment.hpp>
#include <backend/filesystem/filesystem.hpp>
#include <backend/json.hpp>

#include <unordered_map>
#include <string>
#include <mutex>
#include <optional>

class PublicSettings
{
public:
    /**
     *  Loads them from $home/.minIDE/public_settings.json
     */
    void load();

    /**
     *  Saves them from $home/.minIDE/public_settings.json
     */
    void save();

    /**
     *  Swap for copy
     */
    friend void swap(PublicSettings& lhs, PublicSettings& rhs);

    /**
     *  ctor
     */
    PublicSettings();

    /**
     *  dtor
     */
    ~PublicSettings();

    // copy and move
    PublicSettings(PublicSettings const&);
    PublicSettings(PublicSettings&&);
    PublicSettings& operator=(PublicSettings);
    PublicSettings& operator=(PublicSettings&&);

    /**
     *  Get path to settings file
     */
    sfs::path getFileName() const;

    /**
     *  Get all saved environments
     */
    std::unordered_map <std::string, SettingParts::Environment> environments() const;

    /**
     *  Set (all) environments
     */
    void setEnvironments(std::unordered_map <std::string, SettingParts::Environment> const& envs);

    /**
     *  Get an environment by name and resolve inheritance.
     */
    std::optional <std::unordered_map <std::string, std::string>> compileEnvironment(std::string const& name) const;

    /**
     *  nlohnmann::json convertible
     */
    friend void to_json(json& j, PublicSettings const& settings);

    /**
     *  nlohnmann::json convertible
     */
    friend void from_json(json const& j, PublicSettings& settings);

private:
    mutable std::mutex memoSaver_;
    mutable sfs::path homeMemo_;

private:
    std::unordered_map <std::string, SettingParts::Environment> environments_;
};
