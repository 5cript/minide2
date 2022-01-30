#pragma once

#include "session/session.hpp"
#include "routers/router_base.hpp"

#include <iostream>

#include <attender/attender.hpp>
#include <attender/session/session_manager.hpp>
#include <attender/session/uuid_session_cookie_generator.hpp>
#include <attender/session/authorizer_interface.hpp>
#include <attender/session/basic_authorizer.hpp>

void setupLog();
void setupSignalHandler();
void setupCrashHandler();
void buildDirectoryStructure();
void testLua();

class AuthenticateEveryone : public attender::basic_authorizer
{
    std::string realm() const override
    {
        return "MinIDE_Realm";
    }

    bool accept_authentication(std::string_view user, std::string_view password) override
    {
        // we are accepting anyone :3
        return true;
    }
};

template <typename SessionStoreT, typename ServerT, typename... StoreConstruct>
void installSessionHandler(ServerT& server, std::string const& corsOrigin, StoreConstruct&&... constructionArgs)
{
    server.install_session_control
    ({
        std::make_unique <SessionStoreT>(std::forward <StoreConstruct&&>(constructionArgs)...),
        std::make_unique <AuthenticateEveryone>(),
        "aSID",
        true,
        []()
        {
            attender::cookie c;
            c.set_same_site("None");
            return c;
        }(),
        [corsOrigin](auto req, auto res)
        {
            Routers::enable_cors(req, res, corsOrigin);
        },
        true,
        "/api/authenticate"
    });
}

