#include <backend/server/api/user.hpp>

namespace Api
{
    bool User::authenticate(json const& payload)
    {
        return payload.contains("user") && payload.contains("password") &&
            payload["user"].get<std::string>() == "dummy";
    }
}