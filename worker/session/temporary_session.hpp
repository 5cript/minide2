#pragma once

#include "session.hpp"
#include <attender/attender.hpp>

class TemporarySession : public Session
{
public:
    using server_type = attender::http_server;

public:
    explicit TemporarySession(server_type* server, Session&& sess);
    TemporarySession(TemporarySession const&) = delete;
    TemporarySession& operator=(TemporarySession const&) = delete;
    TemporarySession(TemporarySession&&) = default;
    TemporarySession& operator=(TemporarySession&&) = default;

    /**
     *  Used to be automatic, but could race or be in improper order.
     *  ASIO calls can be synchronous as it seems, if they're small.
     *  Which then can lead to a state that is unmodified save after the modded one.
     */
    void save();

    void save_partial(std::function <void(Session& toSave, Session const& toReadFrom)> const& extractor);

    ~TemporarySession();

private:
    server_type* server_;
};
