#pragma once

#include "../session/session.hpp"
#include "../session/temporary_session.hpp"
#include "channel.hpp"

#include <attender/websocket/server/session_base.hpp>
#include <attender/session/session_manager.hpp>
#include <automata/automata.hpp>
#include <attender/encoding/streaming_producer.hpp>
#include <attender/encoding/brotli.hpp>
#include <attender/http/request_header.hpp>

#include <memory>
#include <string>
#include <variant>

namespace Streaming
{
    class WebsocketStreamer;

    class StreamSession : public attender::websocket::session_base
    {
    public:
        StreamSession(attender::websocket::connection*);

        void setup
        (
            WebsocketStreamer* streamer,
            unsigned int id,
            std::weak_ptr <attender::session_manager> sessionManager,
            bool compressed,
            attender::request_header header
        );

        void on_close() override;
        void on_text(std::string_view data) override;
        void on_binary(char const* begin, std::size_t amount) override;
        void on_error(boost::system::error_code ec, char const* where) override;
        void on_write_complete(std::size_t bytes_transferred) override;

        Streaming::StreamChannel channel() const;

        template <typename MessageT>
        void writeMessage(MessageT const& msg)
        {
            std::visit([&msg](auto& producer)
            {
                StreamerBase::writeMessage
                (
                    *producer,
                    msg
                );
            }, producer_);
        }

        std::string host() const;

    private:
        std::optional <Session> getSession();
        void saveSessionPartially
        (
            Session& session,
            std::function <void(Session& toSave, Session const& toReadFrom)> const& extractor
        );

    private:
        WebsocketStreamer* streamer_;
        Streaming::StreamChannel channel_;
        unsigned int streamId_;
        std::weak_ptr <attender::session_manager> sessionManager_;
        MiniAutomata::Automaton communaton_;
        std::string sessionId_; // the real session
        std::string lastReceivedText_;
        std::variant <
            std::unique_ptr <attender::streaming_producer>,
            std::unique_ptr <attender::brotli_encoder>
        > producer_;
        attender::request_header handshakeHeader_;
    };
}
