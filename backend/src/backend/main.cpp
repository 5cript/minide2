#include <backend/main.hpp>

#include <backend/filesystem/filesystem.hpp>
#include <backend/termination_handler.hpp>
#include <backend/server/backend_control.hpp>
#include <backend/config.hpp>
#include <backend/log.hpp>
#include <special-paths/special_paths.hpp>
#include <attender/io_context/managed_io_context.hpp>
#include <attender/io_context/thread_pooler.hpp>
#include <backend/plugin_system/init.hpp>

#include <iostream>

using namespace std::string_literals;

int main(int argc, char* argv[])
{
    setupLog();
    setupCrashHandler();
    buildDirectoryStructure();

    // Load Config
    Config config;

    // Plugins
    PluginSystem::GlobalInit v8Init(argv[0]);

    // io_context
    attender::managed_io_context <attender::thread_pooler> context
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
            LOG() << "Uncaught exception in thread " << sstr.str() << ": " << exc.what() << ".\n";
            std::exit(1);
        }
    };

    // Websocket Server
    std::shared_ptr<BackendControl> wsServer = std::make_shared<BackendControl>(context.get_io_service());
    wsServer->start(std::to_string(config.websocketPort));
    LOG() << "Running ws server on " << config.websocketPort << ".\n";

    // Wait for Enter
    std::cin.get();
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
    log.setConcise(true);
    log.configureProjectMainFile(__FILE__);
    log.setTerminalEnabled(true);
    log.open(
        logPath(),
        10
    );
    LOG() << "Build Time and Date: " << __DATE__ << " " << __TIME__ << ".\n";
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
    createAndRecheck(minIdeUser / "toolbar_persistence");
}