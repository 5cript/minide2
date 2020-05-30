#pragma once

#include "stream_message.hpp"

#include <list>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace Streaming
{
    struct ReferenceCountedMessage
    {
        Message msg;
        mutable std::atomic <std::size_t> refCount;

        ReferenceCountedMessage(Message&& msg, std::size_t count)
            : msg{std::move(msg)}
            , refCount{count}
        {
        }
    };

    class StreamQueue
    {
    public:
        using id_type = unsigned int;
        using message_cache_type = std::list <ReferenceCountedMessage>;

    public:
        /**
         *  Allow a single listener to pop a message.
         *  You can safely read from the message behind the iterator, because the underlying data is never written to
         *  after first write. Only the reference count is touched, which is atomic. The data is removed when all receivers unreffed their access to a message.
         */
        message_cache_type::const_iterator popMessage(id_type retriever);

        /**
         *  Retrieve the amount of consumeable messages for id.
         */
        std::size_t consumeableCount(id_type id);

        /**
         *  Broadcast message to every listener.
         */
        void broadcastMessage(Message&& msg);

        /**
         *  Send message to given id.
         */
        void sendMessage(id_type id, Message&& msg);

        /**
         *  Insert a stream listener.
         */
        void insertRetriever(id_type id);

        /**
         *  Remove a stream listener.
         */
        void eraseRetriever(id_type id);

        void unrefMessage(message_cache_type::const_iterator msgIter);

    private:
        std::unordered_map <id_type, std::list <message_cache_type::const_iterator>> assignments_;
        std::list <ReferenceCountedMessage> messageCache_;
        std::mutex assignmentLock_;
    };
}
