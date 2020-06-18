#include "common_state_setup.hpp"

#include "../filesystem/filesystem.hpp"
#include "../json.hpp"
#include "../log.hpp"

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

        loadLibrariesFromHome(state);
    }
//---------------------------------------------------------------------------------------------------------------------
    void loadLibrariesFromHome(sol::state& state)
    {
        auto luaHome = sfs::path{SpecialPaths::getHome()} / ".minIDE" / "lua";

        auto searchPathFile = luaHome / "search_paths.json";
        if (!sfs::exists(searchPathFile))
            return;

        try
        {
            std::ifstream reader{searchPathFile, std::ios_base::binary};
            if (!reader.good())
            {
                LOG() << "could not open lua search path file, even though it exists\n";
                return;
            }

            auto j = json::parse(reader);
            auto paths = j["paths"].get<std::vector <std::string>>();
            for (auto const& p : paths)
                addToPackagePath(state, luaHome / p);
        }
        catch(std::exception const& exc)
        {
            LOG() << exc.what() << "\n";
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void addToPackagePath(sol::state& state, sfs::path const& toAdd)
    {
        const std::string package_path = state["package"]["path"];
        std::string path = state["package"]["path"];
        path =
            path +
            (!path.empty() ? ";" : "") +
            (toAdd / "?.lua").string()
        ;
        state["package"]["path"] = path;
    }
//#####################################################################################################################
}
