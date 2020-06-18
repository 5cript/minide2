#pragma once

#include <sol/sol.hpp>
#include <mutex>

struct StateCollection
{
    using mutex_type = std::recursive_mutex;

    sol::state lua;
    mutex_type globalMutex;
};
