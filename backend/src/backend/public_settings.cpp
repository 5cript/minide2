#include <backend/public_settings.hpp>

#include <special-paths/special_paths.hpp>

#include <fstream>

using namespace std::string_literals;

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
std::optional <std::unordered_map <std::string, std::string>> PublicSettings::compileEnvironment(std::string const& name) const
{
    auto findEnv = [this](std::string const& name) -> std::optional <SettingParts::Environment>
    {
        auto iter = environments_.find(name);
        if (iter == std::end(environments_))
            return std::nullopt;

        return {iter->second};
    };

    auto optEnv = findEnv(name);
    if (!optEnv)
        return std::nullopt;

    auto env = optEnv.value();
    auto envCpy = env;
    for (auto const& [orderKey, inherits] : envCpy.inherits)
    {
        auto inheritedEnv = findEnv(inherits);
        if (!inheritedEnv)
        {
            throw std::runtime_error
            (
                "environment inherits other environment that wasn't found"s +
                "{\"inherited\":\""s + inherits + "\"}"
            );
        }

        bool sensitive = true;
        // This could be wrong, cause dependent on Filesystem?
#ifdef _WIN32
        sensitive = false;
#endif
        env = inheritedEnv.value().merge(env, sensitive);
    }
    char pathSplit = SettingParts::Environment::linuxPathSplit;
#ifdef _WIN32
    pathSplit = SettingParts::Environment::windowsPathSplit;
#endif // _WIN32
    return env.compile(pathSplit);
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
