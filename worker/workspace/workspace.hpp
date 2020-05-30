#pragma once

#include "../json.hpp"

struct WorkspaceInfo : public JsonSerializable
{
    std::string root;
    std::string activeProject;

    std::string toJson() const override;
};

//ADAPT(WorkspaceInfo, root, activeProject)
