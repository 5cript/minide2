#pragma once

#include "../json.hpp"
#include "../filesystem/filesystem.hpp"
#include "../filesystem/relations.hpp"

struct WorkspaceInfo : public JsonSerializable
{
    // Path to workspace root
    sfs::path root;

    /// return jail helper from root
    Filesystem::Jail rootJail() const;

    // Path to active project, not relative
    sfs::path activeProject;

    std::string toJson() const override;
};

//ADAPT(WorkspaceInfo, root, activeProject)
