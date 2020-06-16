#include "public_settings.hpp"

#include <special-paths/special_paths.hpp>

#include <fstream>

//#####################################################################################################################
void PublicSettings::load()
{
    auto file = getFileName();
    if (sfs::exists(file))
    {
        std::ifstream reader{file.string(), std::ios_base::binary};
        if (!reader.good())
            throw std::runtime_error("cannot open public settings file for reading");
        json::parse(reader)["public"].get_to(*this);
    }
    else
        save();
}
//---------------------------------------------------------------------------------------------------------------------
void PublicSettings::save()
{
    auto file = getFileName();
    std::ofstream writer{file.string(), std::ios_base::binary};
    if (!writer.good())
        throw std::runtime_error("cannot open public settings file for writing");
    json j;
    j["public"] = *this;
    writer << j.dump(4);
}
//---------------------------------------------------------------------------------------------------------------------
sfs::path PublicSettings::getFileName() const
{
    std::lock_guard <std::mutex> memoGuardian{memoSaver_};

    if (homeMemo_.empty())
    {
        auto home = sfs::path{SpecialPaths::getHome()};
        if (!sfs::exists(home / ".minIDE"))
        {
            sfs::create_directory(home / ".minIDE");
        }
        homeMemo_ = home / ".minIDE" / "public_persistence.json";
    }
    return homeMemo_;
}
//---------------------------------------------------------------------------------------------------------------------
std::unordered_map <std::string, SettingParts::Environment> PublicSettings::environments() const
{
    return environments_;
}
//---------------------------------------------------------------------------------------------------------------------
void PublicSettings::setEnvironments(std::unordered_map <std::string, SettingParts::Environment> const& envs)
{
    environments_ = envs;
    save();
}
//---------------------------------------------------------------------------------------------------------------------
PublicSettings::PublicSettings() = default;
//---------------------------------------------------------------------------------------------------------------------
PublicSettings::~PublicSettings() = default;
//---------------------------------------------------------------------------------------------------------------------
PublicSettings::PublicSettings(PublicSettings const& other)
    : environments_{other.environments_}
{
}
//---------------------------------------------------------------------------------------------------------------------
PublicSettings::PublicSettings(PublicSettings&& other)
    : environments_{std::move(other.environments_)}
{
}
//---------------------------------------------------------------------------------------------------------------------
PublicSettings& PublicSettings::operator=(PublicSettings other)
{
    swap(*this, other);
    return *this;
}
//---------------------------------------------------------------------------------------------------------------------
PublicSettings& PublicSettings::operator=(PublicSettings&& other)
{
    environments_ = std::move(other.environments_);
    return *this;
}
//---------------------------------------------------------------------------------------------------------------------
void swap(PublicSettings& lhs, PublicSettings& rhs)
{
    using std::swap;

    swap(lhs.environments_, rhs.environments_);
}
//#####################################################################################################################
void to_json(json& j, PublicSettings const& settings)
{
    j = json
    {
        {"environments", settings.environments_}
    };
}
//---------------------------------------------------------------------------------------------------------------------
void from_json(json const& j, PublicSettings& settings)
{
    j["environments"].get_to(settings.environments_);
}
//#####################################################################################################################
