#include "stream_session.hpp"
#include "ws_streamer.hpp"
#include "common_messages/inline_message.hpp"
#include "../log.hpp"
#include "../session/session_storage.hpp"

#include <iostream>

using namespace std::string_literals;

namespace Streaming
{
//#####################################################################################################################
    StreamSession::StreamSession(attender::websocket::connection* connection)
        : session_base{connection}
        , streamer_{nullptr}
        , streamId_{}
        , sessionManager_{}
        , communaton_{}
        , sessionId_{}
        , lastReceivedText_{}
        , producer_{}
    {

        communaton_ = MiniAutomata::makeAutomaton()
            << "Initial"
            << "Authentication"
            << "Listening"
        ;

        communaton_ > "Initial" > "Authentication" > "Listening" > "Listening";

        communaton_["Authentication"].bindAction([this](){
            auto obj = json::parse(lastReceivedText_);
            if (!obj.contains("sessionId"))
            {
                writeMessage(Streaming::makeMessage<Streaming::Messages::InlineMessage>("rejected_authentification", json{
                    {"message"s, "sessionId not supplied"s},
                }));
                close();
                return;
            }
            sessionId_ = obj["sessionId"].get<std::string>();
            auto maybeSession = getSession();
            if (!maybeSession)
            {
                writeMessage(Streaming::makeMessage<Streaming::Messages::InlineMessage>("rejected_authentification", json{
                    {"message"s, "there is no session for the given id"s},
                }));
                close();
                return;
            }
            if (handshakeHeader_.get_path() == "/api/wsstreamer/data")
            {
                channel_ = StreamChannel::Data;
                saveSessionPartially(*maybeSession, [this](auto& to, auto const&){
                    to.dataId = streamId_;
                });
            }
            else if (handshakeHeader_.get_path() == "/api/wsstreamer/control")
            {
                channel_ = StreamChannel::Control;
                saveSessionPartially(*maybeSession, [this](auto& to, auto const&){
                    to.controlId = streamId_;
                });
            }
            else
            {
                writeMessage(Streaming::makeMessage<Streaming::Messages::InlineMessage>("rejected_authentification", json{
                    {"message"s, "given websocket path is neither expected data line nor expected control line"s},
                }));
                close();
                return;
            }
            writeMessage(Streaming::makeMessage<Streaming::Messages::InlineMessage>("authentication_accepted"));
            LOG() << "" << streamId_ << " started (session: " << sessionId_ << ")" << "\n";
            communaton_.advance();
        });

        communaton_["Listening"].bindAction([this](){
            LOG() << "Received data from client: " << lastReceivedText_ << "\n";
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamSession::setup
    (
        WebsocketStreamer* stream,
        unsigned int id,
        std::weak_ptr <attender::session_manager> sessionManager,
        bool compressed,
        attender::request_header header
    )
    {
        streamer_ = stream;
        streamId_ = id;
        sessionManager_ = std::move(sessionManager);
        handshakeHeader_ = header;
        if (!compressed)
        {
            producer_ = std::make_unique<attender::streaming_producer>("identity",[](){},[](auto){});
        }
        else
        {
            producer_ = std::make_unique<attender::brotli_encoder>();
        }
        std::visit([compressed, this](auto& producer) {
            producer->set_on_produce_cb([compressed, &producer, this](std::string const& err, bool) {
                if (!err.empty())
                {
                    LOG() << "stream producer error: " << err << "\n";
                    producer->end_production({boost::system::errc::connection_reset, boost::system::system_category()});
                    return;
                }
                else if (producer->complete())
                {
                    producer->end_production({boost::system::errc::connection_reset, boost::system::system_category()});
                    return;
                }

                auto avail = producer->available();
                if (avail == 0)
                {
                    producer->has_consumed(0);
                    return;
                }

                if (compressed)
                {
                    producer->buffer_locked_do([&producer, avail, this]{
                        write_binary(producer->data(), avail);
                    });
                }
                else
                {
                    producer->buffer_locked_do([&producer, avail, this]{
                        write_text({producer->data(), avail});
                    });
                }
            });
        }, producer_);
    }
//---------------------------------------------------------------------------------------------------------------------
    Streaming::StreamChannel StreamSession::channel() const
    {
        return channel_;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string StreamSession::host() const
    {
        if (auto field = handshakeHeader_.get_field("Host"); field)
        {
            return *field;
        }
        return "";
    }
//---------------------------------------------------------------------------------------------------------------------
    std::optional <Session> StreamSession::getSession()
    {
        if (auto manager = sessionManager_.lock(); manager)
        {
            Session s;
            auto status = manager->load_session(sessionId_, &s);
            if (status != attender::session_state::live)
                return std::nullopt;
            return std::optional <Session>{s};
        }
        return std::nullopt;
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamSession::saveSessionPartially
    (
        Session& session,
        std::function <void(Session& toSave, Session const& toReadFrom)> const& extractor
    )
    {
        if (auto manager = sessionManager_.lock(); manager)
        {
            std::cout << reinterpret_cast <unsigned long long>(manager.get()) << "\n";
            manager->get_storage<
                timed_memory_session_storage <attender::uuid_generator, Session>
            >()->partially_save_session(sessionId_, session, extractor);
        }
        else
        {
            LOG() << "could not update session";
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamSession::on_close()
    {
        LOG() << "" << streamId_ << " closed (session: " << sessionId_ << ")" << "\n";
        streamer_->removeConnection(streamId_);
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamSession::on_text(std::string_view data)
    {
        lastReceivedText_ = data;
        communaton_.advance();
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamSession::on_binary(char const* begin, std::size_t amount)
    {
        std::cout << "IMPLEMENT_ME_binary: " << std::string{begin, amount} << "\n";
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamSession::on_error(boost::system::error_code ec, char const* where)
    {
        std::cout << "IMPLEMENT_ME_error: " << ec.message() << " at " << where << "\n";
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamSession::on_write_complete(std::size_t bytes_transferred)
    {
        std::visit([bytes_transferred](auto& producer) {
            producer->has_consumed(bytes_transferred);
        }, producer_);
    }
//#####################################################################################################################
}
