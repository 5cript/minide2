#pragma once

#include "user.hpp"
#include "workspace.hpp"

namespace Backend::Server::Api
{
    struct AllApis
    {
        Workspace* workspace;
        User* user;
    };
}