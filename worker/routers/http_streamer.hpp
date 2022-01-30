#pragma once

#include "router_base.hpp"
#include "streamer_fwd.hpp"

#include "../json.hpp"

#include "../streaming/id.hpp"
#include "../streaming/stream_queue.hpp"
#include "../streaming/channel.hpp"
#include "../streaming/streamer_base.hpp"

#include "../variant.hpp"
#include "../config.hpp"

#include <atomic>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <mutex>
#include <iostream>

namespace Routers
{
    /**
     *  This router is special in that it expects a client to stay connected and never disconnects it right away.
     *  It provides a channel for data coming in from somewhere in the server that has to go to the client.
     *  This allows for server->client communication.
     **/
    class HttpDataStreamer
        : public BasicRouter
        , public Streaming::StreamerBase
    {
    public:
        HttpDataStreamer(CommunicationCenter* collection, attender::http_server& server, Config const& config);
        ~HttpDataStreamer();

        void start() override;

        /**
         *  Gracefully shutdowns all ongoing streams
         */
        void shutdownAll() override;

        /**
         *  Broadcast message on control line.
         */
        void broadcast(Streaming::StreamChannel channel, Streaming::Message&& msg) override;

        /**
         *  Send message on channel 'channel' to listener with id.
         *  @return 0 = success, -1 = forbidded by ip, -2 = no listener with id, -3 = invalid channel
         */
        int send(Streaming::StreamChannel channel, std::string const& addr, int id, Streaming::Message&& msg) override;

        /**
         *  Send message formed from json on channel 'channel' to listener with id.
         *  @return 0 = success, -1 = forbidded by ip, -2 = no listener with id, -3 = invalid channel
         */
        int send(Streaming::StreamChannel channel, std::string const& addr, int id, json const& json, std::string const& type) override;

    private:
        void registerRoutes(attender::http_server& server);
        void respondWithError(attender::response_handler* res, char const* msg);
        void readExcept(boost::system::error_code ec);

    private:
        std::atomic_bool endAllStreams_;
        Streaming::Queues <Streaming::StreamChannel::Control> controlStream_;
        Streaming::Queues <Streaming::StreamChannel::Data> dataStream_;
        int maxStreamListeners_;
        bool safeIdChecks_;
    };
}
