#include "workspace_persistence.hpp"

#include "../json.hpp"

#include <fstream>
#include <sstream>

//#####################################################################################################################
WorkspacePersistence::WorkspacePersistence(sfs::path const& root)
    : lastActiveProject{}
    , root_{root}
    , raw_{}
{
}
//---------------------------------------------------------------------------------------------------------------------
bool WorkspacePersistence::save()
{
    auto metaFile = root_ / ".minIDE" / "workspace.json";
    if (!sfs::exists(root_ / ".minIDE"))
        sfs::create_directory(root_ / ".minIDE");
    std::ofstream writer{metaFile.string(), std::ios_base::binary};
    if (!writer.good())
        return false;

    json j;
    j["lastActiveProject"] = lastActiveProject;
    writer << j.dump();
    return true;
}
//---------------------------------------------------------------------------------------------------------------------
bool WorkspacePersistence::load()
{
    auto metaFile = root_ / ".minIDE" / "workspace.json";
    if (!sfs::exists(metaFile))
        return false;

    std::ifstream reader{metaFile.string(), std::ios_base::binary};
    if (!reader.good())
        return false;

    std::stringstream sstr;
    sstr << reader.rdbuf();
    raw_ = sstr.str();

    auto j = json::parse(raw_);

    if (j.contains("lastActiveProject"))
        lastActiveProject = j["lastActiveProject"];

    return true;
}
//---------------------------------------------------------------------------------------------------------------------
std::string WorkspacePersistence::raw() const
{
    return raw_;
}
//#####################################################################################################################
