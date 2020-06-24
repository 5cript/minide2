#pragma once

#include "../json.hpp"
#include "../filesystem/filesystem.hpp"

struct WorkspaceInfo : public JsonSerializable
{
    // Path to workspace root
    sfs::path root;

    // Path to active project, not relative
    sfs::path activeProject;

    std::string toJson() const override;
};

//ADAPT(WorkspaceInfo, root, activeProject)
