#include  "stream_queue.hpp"

namespace Streaming
{
//#####################################################################################################################
    StreamQueue::message_cache_type::const_iterator StreamQueue::popMessage(id_type retriever)
    {
        std::lock_guard <std::mutex> assignmentGuard{assignmentLock_};
        auto accessor = assignments_[retriever].front();
        assignments_[retriever].pop_front();
        return accessor;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::size_t StreamQueue::consumeableCount(id_type id)
    {
        std::lock_guard <std::mutex> assignmentGuard{assignmentLock_};
        return assignments_[id].size();
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamQueue::broadcastMessage(Message&& msg)
    {
        std::lock_guard <std::mutex> assignmentGuard{assignmentLock_};
        auto msgIter = messageCache_.emplace
        (
            std::begin(messageCache_),
            std::move(msg),
            assignments_.size()
        );
        for (auto& [key, value] : assignments_)
            value.push_back(msgIter);
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamQueue::sendMessage(id_type id, Message&& msg)
    {
        std::lock_guard <std::mutex> assignmentGuard{assignmentLock_};
        auto msgIter = messageCache_.emplace
        (
            std::begin(messageCache_),
            std::move(msg),
            1
        );
        auto iter = assignments_.find(id);
        if (iter == assignments_.end())
            throw std::runtime_error("invalid id");

        iter->second.push_back(msgIter);
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamQueue::insertRetriever(id_type id)
    {
        std::lock_guard <std::mutex> assignmentGuard{assignmentLock_};
        assignments_[id] = {};
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamQueue::unrefMessage(message_cache_type::const_iterator msgIter)
    {
        if (--(msgIter->refCount) == 0)
        {
            std::lock_guard <std::mutex> assignmentGuard{assignmentLock_};
            messageCache_.erase(msgIter);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamQueue::eraseRetriever(id_type id)
    {
        std::lock_guard <std::mutex> assignmentGuard{assignmentLock_};
        assignments_.erase(id);
    }
//#####################################################################################################################
}
