#pragma once

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
        SettingsProvider(RouterCollection* collection, attender::tcp_server& server, Config const& config);
        ~SettingsProvider();

        PublicSettings settings() const;

    private:
        void registerRoutes(attender::tcp_server& server);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
