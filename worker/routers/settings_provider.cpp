#include "settings_provider.hpp"

namespace Routers
{
//#####################################################################################################################
    struct SettingsProvider::Implementation
    {
        Config config;
        PublicSettings settings;

        Implementation(Config config)
            : config{std::move(config)}
        {
            settings.load();
        }
    };
//#####################################################################################################################
    SettingsProvider::SettingsProvider(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter(collection)
        , impl_{new SettingsProvider::Implementation(config)}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    SettingsProvider::~SettingsProvider() = default;
//---------------------------------------------------------------------------------------------------------------------
    PublicSettings SettingsProvider::settings() const
    {
        return impl_->settings;
    }
//---------------------------------------------------------------------------------------------------------------------
    void SettingsProvider::registerRoutes(attender::tcp_server& server)
    {
        cors_options(server, "/api/settings/environment/names", "GET");
        server.get("/api/settings/environment/names", [this](auto req, auto res)
        {
            enable_cors(res);

            json j;
            auto names = std::vector <std::string> {};

            auto&& envs = impl_->settings.environments();
            for (auto const& [name, v] : envs)
                names.push_back(name);

            j["names"] = names;

            sendJson(res, j);
        });
    }
//#####################################################################################################################
}
