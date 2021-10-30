#include <backend/server/backend_control.hpp>
#include <backend/server/frontend_user_session.hpp>

#include <backend/log.hpp>

//#####################################################################################################################
BackendControl::BackendControl(boost::asio::io_context* service)
        : server_{service, [](auto const& ec){
            LOG() << ec.message() << "\n";
            std::exit(1);
        }}
        , guard_{}
        , connections_{}
        , generator_{}
{}
//---------------------------------------------------------------------------------------------------------------------
bool BackendControl::removeSession(std::string const& id)
{
    std::scoped_lock lock{guard_};
    return connections_.erase(id) != 0;
}
//---------------------------------------------------------------------------------------------------------------------
void BackendControl::start(std::string const& port)
{
    server_.start([weak = weak_from_this()](std::shared_ptr<attender::websocket::connection> connection) {
        auto shared = weak.lock();
        if (!shared)
        {
            LOG() << "Connection attempt received after server is dead.\n";
            return;
        }

        std::scoped_lock lock{shared->guard_};
        LOG() << "New connection\n";
        //LOG() << "WS_REQUEST_HEADER: \n" << header.to_string() << "\n";
        const auto id = shared->generator_.generate_id();
        connection->create_session<FrontendUserSession>(weak, id).setup();
        shared->connections_[id] = std::move(connection);
    }, port);
}
//#####################################################################################################################