#pragma once

#include "router_base.hpp"
#include "../config.hpp"

#include <memory>
#include <string>
#include <utility>

namespace Routers
{
    class Workspace : public BasicRouter
    {
    public:
        Workspace(RouterCollection* collection, attender::tcp_server& server, Config const& config);
        ~Workspace();

    private:
        void registerRoutes(attender::tcp_server& server);

        /**
         *  Returns an pair of <E, P>. There was an error if E is non-empty
         */
        std::pair <std::string, std::string> verifyPath(std::string path, std::string const& root, bool mustExist = true);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
