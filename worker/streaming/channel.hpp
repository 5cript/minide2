#pragma once

#include "id.hpp"
#include "stream_queue.hpp"

namespace Streaming
{
    enum class StreamChannel
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
}
