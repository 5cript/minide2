#pragma once

#include "router_base.hpp"
#include "../streaming/id.hpp"
#include "../streaming/stream_queue.hpp"

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
    enum StreamChannel
    {
        Control = 0x0,
        Data = 0x1
    };

    template <StreamChannel Channel>
    struct Queues
    {
        constexpr static StreamChannel channel = Channel;
        Streaming::IdProvider idProvider;
        Streaming::StreamQueue queue;

        std::mutex addressRegisterMutex;
        std::unordered_map <Streaming::IdProvider::id_type, std::string> remoteAddresses;
    };

    /**
     *  This router is special in that it expects a client to stay connected and never disconnects it right away.
     *  It provides a channel for data coming in from somewhere in the server that has to go to the client.
     *  This allows for server->client communication.
     **/
    class DataStreamer : public BasicRouter
    {
    public:
        DataStreamer(RouterCollection* collection, attender::tcp_server& server, Config const& config);
        ~DataStreamer();

        /**
         *  Gracefully shutdowns all ongoing streams
         */
        void shutdownAll();

        /**
         *  Broadcast message on control line.
         */
        void broadcast(StreamChannel channel, Streaming::Message&& msg);

        /**
         *  Send message on channel 'channel' to listener with id.
         *  @return 0 = success, -1 = forbidded by ip, -2 = no listener with id, -3 = invalid channel
         */
        int send(StreamChannel channel, std::string const& addr, int id, Streaming::Message&& msg);

    private:
        void registerRoutes(attender::tcp_server& server);
        void respondWithError(attender::response_handler* res, char const* msg);
        void readExcept(boost::system::error_code ec);

        template <typename Producer, typename Message>
        void writeMessage(Producer& produ, Message const& msg)
        {
            std::string j = "0x00000000|";
            j += msg.head->toJson();
            std::stringstream sstr;
            sstr << std::hex << std::setw(8) << std::setfill('0') << (j.size() - 11);
            auto size = sstr.str();
            for (std::size_t i = 0; i != 8; ++i)
                j[2 + i] = size[i];

            produ << j;

            std::visit(overloaded{
                [](std::monostate){},
                [&produ](auto const& dat)
                {
                    std::cout << "writing data of size: " << dat.size() << "\n";

                    produ << dat;
                }
            }, msg.data);

            produ << "\n";
            produ.flush();
        }

    private:
        std::atomic_bool endAllStreams_;
        Queues <StreamChannel::Control> controlStream_;
        Queues <StreamChannel::Data> dataStream_;
        int maxStreamListeners_;
        bool safeIdChecks_;
        Config config_;
    };
}
