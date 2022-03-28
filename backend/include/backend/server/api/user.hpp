#pragma once

#include <backend/json.hpp>

namespace Backend::Server::Api
{
    class User
    {
      public:
        bool authenticate(json const& payload);
    };
}