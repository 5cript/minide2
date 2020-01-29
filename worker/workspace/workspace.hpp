#pragma once

#include "../json.hpp"

struct Workspace : EnableJson <WorkspaceInfo>
{
    std::string root;
    std::string activeProject;
};

ADAPT(Workspace, root, activeProject)
