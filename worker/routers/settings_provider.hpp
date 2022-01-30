#pragma once

#include "settings_provider_fwd.hpp"
#include "router_base.hpp"

#include "../config.hpp"
#include "../public_settings.hpp"

#include <memory>
#include <string>
#include <utility>

namespace Routers
{
    class SettingsProvider : public BasicRouter
    {
    public:
        SettingsProvider(CommunicationCenter* collection, attender::http_server& server, Config const& config);
        ~SettingsProvider();

        PublicSettings settings() const;

    private:
        void registerRoutes(attender::http_server& server);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
