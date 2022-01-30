#pragma once

#include "../config.hpp"
#include "stream_queue.hpp"
#include "stream_message.hpp"
#include "channel.hpp"
#include "../variant.hpp"

#include <string>
#include <sstream>
#include <iomanip>

namespace Streaming
{
    class StreamerBase
    {
    public:
        StreamerBase(Config const& config)
            : config_{config}
        {
        }
        virtual ~StreamerBase() = default;

        virtual void start() = 0;

        /**
         *  Gracefully shutdowns all ongoing streams
         */
        virtual void shutdownAll() = 0;

        /**
         *  Broadcast message on control line.
         */
        virtual void broadcast(Streaming::StreamChannel channel, Streaming::Message&& msg) = 0;

        /**
         *  Send message on channel 'channel' to listener with id.
         *  @return 0 = success, -1 = forbidded by ip, -2 = no listener with id, -3 = invalid channel
         */
        virtual int send(Streaming::StreamChannel channel, std::string const& addr, int id, Streaming::Message&& msg) = 0;

        /**
         *  Send message formed from json on channel 'channel' to listener with id.
         *  @return 0 = success, -1 = forbidded by ip, -2 = no listener with id, -3 = invalid channel
         */
        virtual int send(Streaming::StreamChannel channel, std::string const& addr, int id, json const& json, std::string const& type) = 0;


        template <typename Producer, typename Message>
        static void writeMessage(Producer& produ, Message const& msg)
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

    protected:
        Config config_;
    };
}
