#pragma once

#include "settings/environment.hpp"

#include "filesystem/filesystem.hpp"
#include "json.hpp"

#include <unordered_map>
#include <string>
#include <mutex>

class PublicSettings
{
public:
    void load();
    void save();

    friend void swap(PublicSettings& lhs, PublicSettings& rhs);

    PublicSettings();
    ~PublicSettings();
    PublicSettings(PublicSettings const&);
    PublicSettings(PublicSettings&&);
    PublicSettings& operator=(PublicSettings);
    PublicSettings& operator=(PublicSettings&&);

    sfs::path getFileName() const;

    std::unordered_map <std::string, SettingParts::Environment> environments() const;

    friend void to_json(json& j, PublicSettings const& settings);
    friend void from_json(json const& j, PublicSettings& settings);

private:
    mutable std::mutex memoSaver_;
    mutable sfs::path homeMemo_;

private:
    std::unordered_map <std::string, SettingParts::Environment> environments_;
};
