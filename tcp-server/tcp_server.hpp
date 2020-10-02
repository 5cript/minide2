#pragma once

#include "tcp_connection_manager.hpp"
#include "tcp_connection.hpp"
#include "overloaded.hpp"
#include "stream_connection.hpp"

#include <attender/attender/tcp_server_interface.hpp>
#include <attender/attender/ssl_context_interface.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <functional>

#include <variant>

namespace Tcp
{
    namespace asio = boost::asio;

    class StreamServer : public attender::tcp_server_interface
    {
    public:
        StreamServer
        (
            asio::io_service* service,
            attender::error_callback onError,
            std::function <void(std::shared_ptr <StreamConnection>&&)> onConnect
        );
        ~StreamServer();

        /**
         *  Change server to secure mode.
         */
        void setSecure(std::unique_ptr <attender::ssl_context_interface> context);

        void start(std::string const& port, std::string const& host = "::") override;
        void stop() override;
        attender::settings get_settings() const override;
        attender::connection_manager* get_connections() override;

    private:
        /**
         *  Acceptor for non secure sockets
         */
        template <typename ConnectionT>
        void doAccept()
        {
            if constexpr (std::is_same_v <ConnectionT, SecureConnection>)
                socket_ = std::make_unique <boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(*service_, *sslContext_->get_ssl_context());
            else
                socket_ = boost::asio::ip::tcp::socket{*service_};

            auto checkError = [this](boost::system::error_code ec)
            {
                // the operation was aborted. This usually means, that the server has been destroyed.
                // accessing this is unsafe now.
                if (ec == boost::asio::error::operation_aborted)
                    return false;

                if (!acceptor_.is_open())
                    return false;

                if (ec)
                {
                    onError_(nullptr, ec, {});
                    return false;
                }

                return true;
            };

            std::visit
            (
                overloaded
                {
                    [](std::monostate)
                    {
                    },
                    [this, checkError](boost::asio::ip::tcp::socket& socket)
                    {
                        acceptor_.async_accept
                        (
                            socket,
                            [this, &socket, checkError](auto ec)
                            {
                                if (!checkError(ec))
                                    return;

                                onConnection(connections_.create <Connection> (this, std::move(socket)));

                                doAccept<ConnectionT>();
                            }
                        );
                    },
                    [this, checkError](std::unique_ptr <boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>& socket)
                    {
                        acceptor_.async_accept
                        (
                            attender::internal::get_socket_layer(*socket),
                            [this, &socket, checkError](auto ec)
                            {
                                if (!checkError(ec))
                                    return;

                                onConnection(connections_.create <SecureConnection> (this, socket.release()));

                                doAccept<ConnectionT>();
                            }
                        );
                    }
                },
                socket_
            );
        }

        /**
         *  Called by universal acceptor on new connection
         */
        void onConnection(ConnectionInterface* connection);

    private:
        // asio stuff
        asio::io_service* service_;
        boost::asio::ip::tcp::acceptor acceptor_;
        boost::asio::ip::tcp::endpoint local_endpoint_;

        ConnectionManager connections_;

        attender::error_callback onError_;
        std::function <void(std::shared_ptr <StreamConnection>&&)> onConnect_;
        std::unique_ptr <attender::ssl_context_interface> sslContext_;

        // socket
        std::variant <
            std::monostate,
            boost::asio::ip::tcp::socket,
            std::unique_ptr <boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
        > socket_;
    };
}
