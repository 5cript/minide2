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
        : BasicRouter(collection, &server)
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
        cors_options(server, "/api/settings/environment/names", "GET", impl_->config.corsOption);
        server.get("/api/settings/environment/names", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            json j;
            auto names = std::vector <std::string> {};

            auto&& envs = impl_->settings.environments();
            for (auto const& [name, v] : envs)
                names.push_back(name);

            j["names"] = names;

            sendJson(res, j);
        });

        cors_options(server, "/api/settings/environment/load", "GET", impl_->config.corsOption);
        server.get("/api/settings/environment/load", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            json j;
            auto&& envs = impl_->settings.environments();

            j["environments"] = envs;

            sendJson(res, j);
        });

        cors_options(server, "/api/settings/environment/save", "POST", impl_->config.corsOption);
        server.post("/api/settings/environment/save", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                try
                {
                    if (!body.contains("environments"))
                        return res->status(400).send("need environments in json body");

                    std::unordered_map <std::string, SettingParts::Environment> envs;
                    envs = body["environments"].get<decltype(envs)>();
                    impl_->settings.setEnvironments(envs);

                    res->status(204).end();
                }
                catch(json::exception const& exc)
                {
                    return respondWithError(res, exc.what());
                }
                catch(std::exception const& exc)
                {
                    return respondWithError(res, exc.what());
                }
                catch(...)
                {
                    return respondWithError(res, "something went wrong while reading from the json");
                }
            });
        });
    }
//#####################################################################################################################
}
