#pragma once

#include "state.hpp"
#include "../session/session_obtainer.hpp"
#include "../routers/streamer_fwd.hpp"
#include "../routers/settings_provider_fwd.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <optional>

namespace MinIDE::Scripting
{
    /**
     *  Exposed to lua.
     */
    class LuaSettingsProvider
    {
    public:
        LuaSettingsProvider
        (
            std::weak_ptr <StateCollection> weakStateRef,
            SessionObtainer sessionAccess,
            Routers::DataStreamer* streamer,
            Routers::SettingsProvider* settingsProv
        );
        ~LuaSettingsProvider();

        std::optional <std::unordered_map <std::string, std::string>> environment(std::string const& envName) const;
        std::vector <std::string> environments() const;

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };

    void loadSettingsProvider
    (
        std::weak_ptr <StateCollection> state,
        SessionObtainer sessionAccess,
        Routers::DataStreamer* streamer,
        Routers::SettingsProvider* settingsProv
    );
}
