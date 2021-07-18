#pragma once

#include "../config.hpp"
#include "stream_queue.hpp"
#include "stream_message.hpp"
#include "channel.hpp"
#include "streamer_base.hpp"

#include <attender/websocket/server/server.hpp>
#include <attender/session/uuid_session_cookie_generator.hpp>
#include <attender/session/session_manager.hpp>

#include <unordered_map>
#include <mutex>
#include <memory>

class CommunicationCenter;

namespace Streaming
{

class WebsocketStreamer : public StreamerBase
{
public:
    friend class StreamSession;

    WebsocketStreamer(CommunicationCenter* collection, boost::asio::io_context* service, Config const& config);

    void start() override;

    /**
     *  Set session manager for use.
     */
    void setSessionManager(std::weak_ptr <attender::session_manager>&& sessionManager);

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
    void removeConnection(unsigned int id);

private:
    CommunicationCenter* collection_;
    std::weak_ptr <attender::session_manager> sessionManager_;
    std::mutex connectionGuard_;
    std::unordered_map<unsigned int, std::shared_ptr<attender::websocket::connection>> connections_;
    IdProvider idGenerator_;
    attender::websocket::server ws_;
    Config config_;
    std::string lastReceivedText_;
};

}
