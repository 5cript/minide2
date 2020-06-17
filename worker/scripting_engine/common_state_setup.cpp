#include "common_state_setup.hpp"

#include "../filesystem/filesystem.hpp"

#include <special-paths/special_paths.hpp>

namespace MinIDE::Scripting
{
//#####################################################################################################################
    void commonStateSetup(sol::state& state, bool enableIo, bool enableOs)
    {
        state.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::coroutine,
            sol::lib::string,
            sol::lib::math,
            sol::lib::table,
            sol::lib::debug,
            sol::lib::bit32,
            sol::lib::ffi
        );

        if (enableOs)
            state.open_libraries(sol::lib::io);
        if (enableIo)
            state.open_libraries(sol::lib::os);

        const std::string package_path = state["package"]["path"];
        std::string path = state["package"]["path"];
        path =
            path +
            (!path.empty() ? ";" : "") +
            (sfs::path{SpecialPaths::getHome()} / "lib" / "Penlight" / "lua" / "?.lua").string()
        ;
        state["package"]["path"] = path;
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
