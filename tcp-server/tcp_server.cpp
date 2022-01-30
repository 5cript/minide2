#include "tcp_server.hpp"

namespace Tcp
{
//#####################################################################################################################
    StreamServer::StreamServer
    (
        asio::io_service* service,
        attender::error_callback onError,
        std::function <void(std::shared_ptr <StreamConnection>&&)> onConnect
    )
        : service_{service}
        , acceptor_{*service}
        , local_endpoint_{}
        , connections_{}
        , onError_{std::move(onError)}
        , onConnect_{std::move(onConnect)}
        , sslContext_{}
        , socket_{}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    StreamServer::~StreamServer()
    {
        stop();
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamServer::setSecure(std::unique_ptr <attender::ssl_context_interface> context)
    {
        sslContext_ = std::move(context);
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamServer::start(std::string const& port, std::string const& host)
    {
        stop();

        boost::asio::ip::tcp::resolver resolver{*service_};
        local_endpoint_ = *resolver.resolve(host, port);

        acceptor_.open(local_endpoint_.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(local_endpoint_);
        acceptor_.listen();

        if (sslContext_)
            doAccept<SecureConnection>();
        else
            doAccept<Connection>();
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamServer::onConnection(ConnectionInterface* connection)
    {
        onConnect_(std::make_shared <StreamConnection> (connection));
    }
//---------------------------------------------------------------------------------------------------------------------
    attender::settings StreamServer::get_settings() const
    {
        return {};
    }
//---------------------------------------------------------------------------------------------------------------------
    attender::connection_manager* StreamServer::get_connections()
    {
        return &connections_;
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamServer::stop()
    {
        acceptor_.close();
    }
//#####################################################################################################################
}
