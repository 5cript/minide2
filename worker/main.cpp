#include "filesystem/filesystem.hpp"

// routers
#include "routers/workspace.hpp"
#include "routers/toolbar.hpp"
#include "routers.hpp"

#include "config.hpp"
#include "log.hpp"
#include "termination_handler.hpp"
#include "streaming/common_messages/server_time.hpp"

// FIXME REMOVE
#include "hybrid_read_sink.hpp"

#include <special-paths/special_paths.hpp>
#include <attender/attender.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using namespace std::string_literals;

void setupLog();
void setupCrashHandler();

int main(int argc, char** argv)
{
    setupLog();
    setupCrashHandler();

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
    server.start(std::to_string(config.port));

    // Routings
    using namespace Routers;
    RouterCollection routers{&server, config};

    //Filesystem::DirectoryCache cache{"D:/Development/IDE2/test-project"};
    std::cout << "Waiting for enter\n";
    std::cin.get();

    routers.streamer().shutdownAll();
    std::this_thread::sleep_for(100ms);
}

void setupLog()
{
    auto logPath = []()
    {
        auto home = sfs::path{SpecialPaths::getHome()};
        if (!sfs::exists(home / ".minIDE"))
            sfs::create_directory(home / ".minIDE");
        if (!sfs::exists(home / ".minIDE" / "logs"))
            sfs::create_directory(home / ".minIDE" / "logs");
        return (home / ".minIDE" / "logs" / "log").string();
    };

    // Logging
    auto& log = LOG().log();
    log.configureProjectMainFile(__FILE__);
    log.setTerminalEnabled(true);
    log.open(
        logPath(),
        10
    );
    LOG() << "Build Time and Date: " << __DATE__ << " " << __TIME__ << '\n';
}

void setupCrashHandler()
{
    std::set_terminate(&onTerminate);

    signal(SIGABRT, &onBadSignal);
    signal(SIGFPE, &onBadSignal);
    signal(SIGILL, &onBadSignal);
    signal(SIGSEGV, &onBadSignal);
}
