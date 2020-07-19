#include "project_persistence.hpp"

#include <fstream>

//#####################################################################################################################
ProjectPersistence::ProjectPersistence(sfs::path const& root)
    : splitSourceAndInclude{false}
    , ignoreSeemingSplit{false}
    , root_{root}
    , raw_{}
{

}
//---------------------------------------------------------------------------------------------------------------------
void ProjectPersistence::inject(json const& j)
{
    transferExistingItems(j);
}
//---------------------------------------------------------------------------------------------------------------------
void ProjectPersistence::transferExistingItems(json const& j)
{
    if (j.contains("splitSourceAndInclude"))
        splitSourceAndInclude = j["splitSourceAndInclude"].get<bool>();
    if (j.contains("ignoreSeemingSplit"))
        ignoreSeemingSplit = j["ignoreSeemingSplit"].get<bool>();
}
//---------------------------------------------------------------------------------------------------------------------
bool ProjectPersistence::save() const
{
    auto metaFile = root_ / ".minIDE" / "project.json";
    if (!sfs::exists(root_ / ".minIDE"))
        sfs::create_directory(root_ / ".minIDE");

    std::ofstream writer{metaFile.string(), std::ios_base::binary};
    if (!writer.good())
        return false;

    json j;
    j["splitSourceAndInclude"] = splitSourceAndInclude;
    j["ignoreSeemingSplit"] = ignoreSeemingSplit;
    writer << j.dump();
    return true;
}
//---------------------------------------------------------------------------------------------------------------------
bool ProjectPersistence::load()
{
    auto metaFile = root_ / ".minIDE" / "project.json";
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
std::string ProjectPersistence::raw()
{
    if (raw_.empty())
        load();
    if (raw_.empty())
    {
        if (save())
            load();
    }
    return raw_;
}
//#####################################################################################################################
