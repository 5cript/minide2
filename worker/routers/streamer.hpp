#pragma once

#include "router_base.hpp"

namespace Routers
{
    /**
     *  This router is special in that it expects a client to stay connected and never disconnects it right away.
     *  It provides a channel for data coming in from somewhere in the server that has to go to the client.
     *  This allows for server->client communication.
     **/
    class DataStreamer
    {
        public:
            Toolbar(attender::tcp_server& server);

        private:
            void registerRoutes(attender::tcp_server& server);
            void respondWithError(attender::response_handler* res, char const* msg);
            void readExcept(boost::system::error_code ec);
        };
    };
}
