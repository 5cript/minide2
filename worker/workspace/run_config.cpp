#include "run_config.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

//#####################################################################################################################
RunConfig::RunConfig(sfs::path const& root)
    : root_{root}
    , raw_{}
{

}
//---------------------------------------------------------------------------------------------------------------------
bool RunConfig::load()
{
    auto metaFile = root_ / ".minIDE" / "run.json";
    if (!sfs::exists(metaFile))
        return false;

    std::ifstream reader{metaFile.string(), std::ios_base::binary};
    if (!reader.good())
        return false;

    std::stringstream sstr;
    sstr << reader.rdbuf();
    raw_ = sstr.str();

    auto j = json::parse(raw_);

    transferExistingItems(j);

    return true;
}
//---------------------------------------------------------------------------------------------------------------------
std::string RunConfig::raw() const
{
    return raw_;
}
//---------------------------------------------------------------------------------------------------------------------
void RunConfig::transferExistingItems(json const& j)
{
    j.contains("configurations");
    json configs = j["configurations"];

#define EXTRACT(jso, name, type) \
    jso.contains(name) ? jso[name].get<type>() : type{}

#define EXTRACT_PROFILE(name, type) \
    EXTRACT(config, name, type)

    for (json config : configs)
    {
        RunConfig::Contents::Configuration cfg
        {
            EXTRACT_PROFILE("name", std::string),
            EXTRACT_PROFILE("type", std::string),
            EXTRACT_PROFILE("debugger", std::string),
            EXTRACT_PROFILE("arguments", std::string),
            EXTRACT_PROFILE("executeable", std::string),
        };

        content_.configs.push_back(cfg);
    }

#undef EXTRACT_PROFILE
#undef EXTRACT
}
//---------------------------------------------------------------------------------------------------------------------
std::optional <RunConfig::Contents::Configuration> RunConfig::findProfile(std::string const& name)
{
    auto res = std::find_if(std::begin(content_.configs), std::end(content_.configs), [&name](auto const& elem){
        return elem.name == name;
    });
    if (res == std::end(content_.configs))
        return std::nullopt;
    return *res;
}
//#####################################################################################################################
