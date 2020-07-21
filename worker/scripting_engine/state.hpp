#pragma once

#include <sol/sol.hpp>
#include <mutex>

struct StateCollection
{
    using mutex_type = std::recursive_timed_mutex;

    template <typename... List>
    using guard_type_templ = std::unique_lock <List...>;

    using guard_type = std::unique_lock <mutex_type>;

    sol::state lua;
    mutex_type globalMutex;
};
