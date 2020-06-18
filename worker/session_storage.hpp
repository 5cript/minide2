#pragma once

#include <attender/attender/session/session_storage_interface.hpp>

#include <unordered_map>
#include <chrono>
#include <mutex>

template <typename SessionT>
struct timed_session_wrapper : public SessionT
{
    std::chrono::system_clock::time_point last_access;

    timed_session_wrapper()
        : SessionT{}
        , last_access{std::chrono::system_clock::now()}
    {
    }

    template <typename Id>
    timed_session_wrapper(Id const& id)
        : SessionT{id}
        , last_access{std::chrono::system_clock::now()}
    {
    }

    timed_session_wrapper(timed_session_wrapper const&) = default;

    timed_session_wrapper& operator=(SessionT const& other)
    {
        *this = other;
        last_access = std::chrono::system_clock::now();
        return *this;
    }
    timed_session_wrapper& operator=(timed_session_wrapper const& other) = default;
    timed_session_wrapper& operator=(timed_session_wrapper&& other) = default;
};

template <typename IdGenerator, typename SessionT>
class timed_memory_session_storage : public attender::session_storage_interface
{
public:
    using session_type = timed_session_wrapper <SessionT>;
    using derived_session_type = SessionT;

public:
    timed_memory_session_storage(std::chrono::seconds const& timeout)
        : sessions_{}
        , gen_{}
        , timeout_{timeout}
        , lock_{}
    {
    }
    void clear() override
    {
        std::lock_guard <std::recursive_mutex> guard{lock_};
        sessions_.clear();
    }
    uint64_t size() override
    {
        std::lock_guard <std::recursive_mutex> guard{lock_};
        return sessions_.size();
    }
    std::string create_session() override
    {
        std::lock_guard <std::recursive_mutex> guard{lock_};

        auto id = gen_.generate_id();
        sessions_[id] = session_type{id};
        return id;
    }
    void delete_session(std::string const& id) override
    {
        std::lock_guard <std::recursive_mutex> guard{lock_};

        sessions_.erase(id);
    }
    bool get_session(std::string const& id, attender::session* session) override
    {
        std::lock_guard <std::recursive_mutex> guard{lock_};

        auto iter = sessions_.find(id);
        if (iter == std::end(sessions_))
            return false;

        // timeout
        auto now = std::chrono::system_clock::now();
        if (now - iter->second.last_access > timeout_)
        {
            delete_session(id);
            return false;
        }
        else
            iter->second.last_access = std::chrono::system_clock::now();

        if (session != nullptr)
            *session = iter->second;
        return true;
    }
    bool set_session(std::string const& id, attender::session const& session) override
    {
        std::lock_guard <std::recursive_mutex> guard{lock_};

        auto iter = sessions_.find(id);
        if (iter == std::end(sessions_))
            return false;
        iter->second = *static_cast <session_type const*> (&session);
        return true;
    }
    void remove_timed_out()
    {
        std::lock_guard <std::recursive_mutex> guard{lock_};

        auto now = std::chrono::system_clock::now();
        for (auto i = std::begin(sessions_); i != std::end(sessions_);)
        {
            if (now - i->second.last_access > timeout_)
                i = sessions_.erase(i);
            else
                ++i;
        }
    }

private:
    std::unordered_map <std::string, session_type> sessions_;
    IdGenerator gen_;
    std::chrono::seconds timeout_;
    std::recursive_mutex lock_;
};
