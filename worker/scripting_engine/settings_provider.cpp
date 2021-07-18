#include "settings_provider.hpp"

#include "../streaming/streamer_base.hpp"
#include "../routers/settings_provider.hpp"
#include "../streaming/common_messages/messages_from_lua.hpp"

namespace MinIDE::Scripting
{
    using namespace Routers;
    using namespace std::string_literals;

//#####################################################################################################################
    struct LuaSettingsProvider::Implementation
    {
        std::weak_ptr <StateCollection> weakStateRef;
        SessionObtainer sessionAccess;
        Streaming::StreamerBase* streamer;
        Routers::SettingsProvider* settingsProv;

        Implementation
        (
            std::weak_ptr <StateCollection>&& stateRef,
            SessionObtainer&& sessionAccess,
            Streaming::StreamerBase* streamer,
            Routers::SettingsProvider* settingsProv
        )
            : weakStateRef{std::move(stateRef)}
            , sessionAccess{std::move(sessionAccess)}
            , streamer{streamer}
            , settingsProv{settingsProv}
        {
        }
    };
//#####################################################################################################################
    LuaSettingsProvider::LuaSettingsProvider
    (
        std::weak_ptr <StateCollection> weakStateRef,
        SessionObtainer sessionAccess,
        Streaming::StreamerBase* streamer,
        Routers::SettingsProvider* settingsProv
    )
        : impl_{new LuaSettingsProvider::Implementation
        (
            std::move(weakStateRef),
            std::move(sessionAccess),
            streamer,
            settingsProv
        )}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    LuaSettingsProvider::~LuaSettingsProvider() = default;
//---------------------------------------------------------------------------------------------------------------------
    std::vector <std::string> LuaSettingsProvider::environments() const
    {
        auto&& envs = impl_->settingsProv->settings().environments();

        std::vector <std::string> vec;
        for (auto const& [env, value] : envs)
            vec.push_back(env);

        return vec;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::optional <std::unordered_map <std::string, std::string>> LuaSettingsProvider::environment(std::string const& envName) const
    {
        auto s = impl_->sessionAccess.session();
        if (!s)
            return std::nullopt;

        try
        {
            return impl_->settingsProv->settings().compileEnvironment(envName);
        }
        catch(std::exception const& exc)
        {
            impl_->streamer->send
            (
                Streaming::StreamChannel::Control,
                s.value().remoteAddress,
                s.value().controlId,
                Streaming::makeMessage<Streaming::Messages::LuaErrorMessage>
                (
                    std::string{exc.what()},
                    "{}",
                    Streaming::Messages::ErrorTypes::Precondition
                )
            );
        }
        return std::nullopt;
    }
//#####################################################################################################################
    void loadSettingsProvider
    (
        std::weak_ptr <StateCollection> state,
        SessionObtainer sessionAccess,
        Streaming::StreamerBase* streamer,
        Routers::SettingsProvider* settingsProv
    )
    {
        auto strongRef = state.lock();
        if (!strongRef)
            return;
        std::lock_guard <StateCollection::mutex_type> {strongRef->globalMutex};

        auto usertype = strongRef->lua.new_usertype<LuaSettingsProvider>
        (
            "SettingsProvider",
            "new", sol::initializers
            (
                [state, sessionAccess{std::move(sessionAccess)}, streamer, settingsProv](LuaSettingsProvider& p) -> void
                {
                    new (&p) LuaSettingsProvider(state, std::move(sessionAccess), streamer, settingsProv);
                }
            ),
            "environment", &LuaSettingsProvider::environment,
            "environments", [](LuaSettingsProvider& p){ return sol::as_table(p.environments()); }
        );
    }
//#####################################################################################################################
}
