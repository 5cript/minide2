#pragma once

#include "../json.hpp"

struct WorkspaceInfo : public EnableJson <WorkspaceInfo>
{
    std::string root;
    std::string activeProject;
};

//ADAPT(WorkspaceInfo, root, activeProject)
