#include "main.hpp"

#include "filesystem/filesystem.hpp"
#include "scripting_engine/process.hpp"
#include "scripting_engine/common_state_setup.hpp"
#include "scripting_engine/script.hpp"

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
#include <sstream>

using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace attender;

int main(int argc, char** argv)
{
    setupLog();
    setupCrashHandler();
    buildDirectoryStructure();

    // TODO: REMOVE ME
    testLua();
    return 0;


    // Load Config
    Config config;

    // an io_service wrapper for boost::asio::io_service.
    // you can provide your own implementation, by subclassing "attender::async_model".
    managed_io_context <thread_pooler> context
    {
        static_cast <std::size_t> (config.httpThreadCount),
        []()
        {
            setupSignalHandler();
        },
        [](std::exception const& exc)
        {
            std::stringstream sstr;
            sstr << std::this_thread::get_id();
            LOG() << "Uncaught exception in thread " << sstr.str() << ": " << exc.what() << "\n";
        }
    };

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

void setupSignalHandler()
{
    signal(SIGABRT, &onBadSignal);
    signal(SIGFPE, &onBadSignal);
    signal(SIGILL, &onBadSignal);
    signal(SIGSEGV, &onBadSignal);
}

void setupCrashHandler()
{
    std::set_terminate(&onTerminate);
    setupSignalHandler();
}

void buildDirectoryStructure()
{
    auto createAndRecheck = [](sfs::path const& where)
    {
        if (!sfs::exists(where))
            sfs::create_directory(where);

        if (!sfs::exists(where))
            throw std::runtime_error("Cannot create path: "s + where.string());
    };

    auto home = sfs::path{SpecialPaths::getHome()};
    auto minIdeUser = home / ".minIDE";

    createAndRecheck(minIdeUser);
    createAndRecheck(minIdeUser / "logs");
    createAndRecheck(minIdeUser / "lua");
    createAndRecheck(minIdeUser / "lua" / "lib");
    createAndRecheck(minIdeUser / "toolbars");
}

void testLua()
{
    using namespace MinIDE::Scripting;

    try
    {
        sol::state lua;

        commonStateSetup(lua, true);
        auto processStore = loadProcessUtility(lua);

        lua["debugging"] = false;

        auto home = sfs::path{SpecialPaths::getHome()};
        auto cmakeScript = home / ".minIDE" / "toolbars" / "cmake" / "main.lua";

        std::cout << cmakeScript.string() << "\n";

        lua.script(Script{cmakeScript}.script());

        sol::protected_function runAction = lua["runAction"];
        if (!runAction.valid())
            throw std::runtime_error("script does not have 'runAction' function");

        runAction(0);
    }
    catch(std::exception const& exc)
    {
        std::cout << exc.what() << "\n";
    }

    std::cin.get();
}
