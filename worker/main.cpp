#include "filesystem/directory_cache.hpp"

// routers
#include "routers/workspace.hpp"
#include "routers/toolbar.hpp"
#include "routers.hpp"

#include "config.hpp"
#include "streaming/common_messages/server_time.hpp"

#include <attender/attender.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

int main(int argc, char** argv)
{
    using namespace attender;

    // Load Config
    Config config;

    // an io_service wrapper for boost::asio::io_service.
    // you can provide your own implementation, by subclassing "attender::async_model".
    managed_io_context <thread_pooler> context;

    // create a server
    tcp_server server(context.get_io_service(),
        [](auto* connection, auto const& ec, auto const& exc) {
            // some error occured. (this is not thread safe)
            // You MUST check the error code here, because some codes mean, that the connection went kaputt!
            // Accessing the connection might be invalid then / crash you.
            if (ec.value() == boost::system::errc::protocol_error)
            {
                std::cout << connection->get_remote_address() << ":" << connection->get_remote_port() << "\n";
            }
            std::cerr << ec << " " << exc.what() << "\n";
        }
    );

    // start server on port 80. Numbers are also valid
    server.start(std::to_string(43255));

    // Routings
    using namespace Routers;
    RouterCollection routers{&server, config};

    //Filesystem::DirectoryCache cache{"D:/Development/IDE2/test-project"};
    std::cout << "Waiting for enter\n";
    std::cin.get();

    routers.streamer().broadcast(StreamChannel::Control, new Streaming::Messages::ServerTime());
    std::this_thread::sleep_for(500ms);

    routers.streamer().shutdownAll();
    std::this_thread::sleep_for(100ms);
}
