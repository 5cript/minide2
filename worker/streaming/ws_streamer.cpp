#include "ws_streamer.hpp"
#include "../log.hpp"
#include "stream_session.hpp"
#include "common_messages/inline_message.hpp"

namespace Streaming
{
//#####################################################################################################################
    WebsocketStreamer::WebsocketStreamer(CommunicationCenter* collection, boost::asio::io_context* service, Config const& config)
        : StreamerBase(config)
        , collection_{collection}
        , connectionGuard_{}
        , connections_{}
        , idGenerator_{}
        , ws_{service, [](auto ec){
            if (ec.value() != 10009) // FIXME: windows only, what is the proper code?
                LOG() << "error in ws: " << ec.message() << "\n";
        }}
        , config_{config}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void WebsocketStreamer::start()
    {
        ws_.start([this](std::shared_ptr<attender::websocket::connection> connection, auto header){
            LOG() << "WS_REQUEST_HEADER: \n" << header.to_string() << "\n";
            std::scoped_lock lock{connectionGuard_};
            auto const id = idGenerator_.acquireId();
            connection->create_session<StreamSession>().setup(this, id, sessionManager_, false, header);
            connections_[id] = std::move(connection);
            return true;
        }, std::to_string(config_.streamingPort));
    }
//---------------------------------------------------------------------------------------------------------------------
    void WebsocketStreamer::setSessionManager(std::weak_ptr <attender::session_manager>&& sessionManager)
    {
        sessionManager_ = std::move(sessionManager);
    }
//---------------------------------------------------------------------------------------------------------------------
    void WebsocketStreamer::removeConnection(unsigned int id)
    {
        std::scoped_lock lock{connectionGuard_};
        auto iter = connections_.find(id);
        if (iter != std::end(connections_))
        {
            connections_.erase(iter);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void WebsocketStreamer::shutdownAll()
    {
        ws_.stop();
        std::scoped_lock lock{connectionGuard_};
        connections_.clear();
    }
//---------------------------------------------------------------------------------------------------------------------
    void WebsocketStreamer::broadcast(Streaming::StreamChannel channel, Streaming::Message&& msg)
    {
        for (auto const& [id, connection] : connections_)
        {
            auto& session = *connection->get_session<StreamSession>();
            if (session.channel() != channel)
                continue;
            session.writeMessage(msg);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    int WebsocketStreamer::send(Streaming::StreamChannel channel, std::string const& addr, int id, Streaming::Message&& msg)
    {
        auto iter = connections_.find(id);
        if (iter != std::end(connections_))
        {
            auto& session = *iter->second->get_session<StreamSession>();
            /*
            if (session.host() != addr)
            {
                LOG() << "stream origin check failed\n";
                return -1;
            }
            */
            if (session.channel() != channel)
            {
                LOG() << "channel type mismatch detected\n";
                return -3;
            }
            session.writeMessage(msg);
            return 0;
        }
        else
        {
            LOG() << "there is no such connection with id " << id << "\n";
            return -2;
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    int WebsocketStreamer::send(Streaming::StreamChannel channel, std::string const& addr, int id, json const& json, std::string const& type)
    {
        return send(channel, addr, id, Streaming::makeMessage<Streaming::Messages::InlineMessage>(type, json));
    }
//#####################################################################################################################
}
