#pragma once

#include "user.hpp"
#include "workspace.hpp"

namespace Api
{

    struct AllApis
    {
        Workspace* workspace;
        User* user;
    };

}