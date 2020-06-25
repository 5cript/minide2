#pragma once

#include "state.hpp"
#include "../session/session_obtainer.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace MinIDE::Scripting
{
    /**
     *  Exposed to lua.
     */
    class LuaStreamer
    {
    public:
        LuaStreamer(std::weak_ptr <StateCollection> weakStateRef, SessionObtainer sessionAccess);
        ~LuaStreamer();

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };

    void loadStreamerAccess(std::weak_ptr <StateCollection> state, SessionObtainer sessionAccess);
}
