#include "streamer_access.hpp"

namespace MinIDE::Scripting
{
//#####################################################################################################################
    struct LuaStreamer::Implementation
    {
        std::weak_ptr <StateCollection> weakStateRef;
        SessionObtainer sessionAccess;

        Implementation(std::weak_ptr <StateCollection>&& stateRef, SessionObtainer&& sessionAccess)
            : weakStateRef{std::move(stateRef)}
            , sessionAccess{std::move(sessionAccess)}
        {
        }
    };
//#####################################################################################################################
    LuaStreamer::LuaStreamer(std::weak_ptr <StateCollection> weakStateRef, SessionObtainer sessionAccess)
        : impl_{new LuaStreamer::Implementation(std::move(weakStateRef), std::move(sessionAccess))}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    LuaStreamer::~LuaStreamer() = default;
//#####################################################################################################################
    void loadStreamerAccess(std::weak_ptr <StateCollection> state, SessionObtainer sessionAccess)
    {
        auto strongRef = state.lock();
        if (!strongRef)
            return;
        std::lock_guard <StateCollection::mutex_type> {strongRef->globalMutex};

        auto usertype = strongRef->lua.new_usertype<LuaStreamer>
        (
            "Streamer",
            "new", sol::initializers
            (
                [state](LuaStreamer& p) -> void
                {
                    new (&p) LuaStreamer(state);
                }
            )
        );
    }
//#####################################################################################################################
}
