#pragma once

#include <backend/json.hpp>

namespace Api
{
    class User
    {
    public:
        bool authenticate(json const& payload);
    };
}