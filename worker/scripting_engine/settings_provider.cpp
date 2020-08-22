#include "settings_provider.hpp"

#include "../routers/streamer.hpp"
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
        Routers::DataStreamer* streamer;
        Routers::SettingsProvider* settingsProv;

        Implementation
        (
            std::weak_ptr <StateCollection>&& stateRef,
            SessionObtainer&& sessionAccess,
            Routers::DataStreamer* streamer,
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
        Routers::DataStreamer* streamer,
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
    std::optional <std::map <std::string, std::string>> LuaSettingsProvider::environment(std::string const& envName) const
    {
        auto&& envs = impl_->settingsProv->settings().environments();

        auto findEnv = [&envs](std::string const& name) -> std::optional <SettingParts::Environment>
        {
            auto iter = envs.find(name);
            if (iter == std::end(envs))
                return std::nullopt;

            return {iter->second};
        };

        auto optEnv = findEnv(envName);
        if (!optEnv)
            return std::nullopt;

        auto s = impl_->sessionAccess.session();
        if (!s)
            return std::nullopt;

        auto env = optEnv.value();
        auto envCpy = env;
        for (auto const& [orderKey, inherits] : envCpy.inherits)
        {
            auto inheritedEnv = findEnv(inherits);
            if (!inheritedEnv)
            {
                impl_->streamer->send
                (
                    StreamChannel::Control,
                    s.value().remoteAddress,
                    s.value().controlId,
                    Streaming::makeMessage<Streaming::Messages::LuaErrorMessage>
                    (
                        "environment inherits other environment that wasn't found",
                        "{\"inherited\":\""s + inherits + "\"}",
                        Streaming::Messages::ErrorTypes::Precondition
                    )
                );
            }

            bool sensitive = true;
// This could be wrong, cause dependent on Filesystem?
#ifdef _WIN32
            sensitive = false;
#endif
            env = inheritedEnv.value().merge(env, sensitive);
        }
        char pathSplit = SettingParts::Environment::linuxPathSplit;
#ifdef _WIN32
        pathSplit = SettingParts::Environment::windowsPathSplit;
#endif // _WIN32
        return env.compile(pathSplit);
    }
//#####################################################################################################################
    void loadSettingsProvider
    (
        std::weak_ptr <StateCollection> state,
        SessionObtainer sessionAccess,
        Routers::DataStreamer* streamer,
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
