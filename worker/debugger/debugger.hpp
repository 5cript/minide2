#pragma once

#include "../streaming/streamer_base.hpp"

#include <debugger-interface/debugger.hpp>
#include <debugger-interface/debugger_interface.hpp>
#include <debugger-interface/commands/mi_command.hpp>

#include "../workspace/run_config.hpp"
#include "../json.hpp"

#include <string>
#include <memory>
#include <mutex>
#include <optional>
#include <map>
#include <string>
#include <thread>
#include <functional>

class Debugger : public DebuggerInterface::PickyListener
{
public:
    /**
     *  Creates a debugger instance.
     *  @param usedConfig the used run config for this instance. Contains information about how to run it.
     *         make sure the passed config does not contain any variables anymore like "${ProjectRoot}".
     *  @param instanceId Instance id is stored to forward it to the ui.
     */
    Debugger
    (
        Streaming::StreamerBase* streamer,
        std::string const& remoteAddress,
        int controlId,
        RunConfig const& usedConfig,
        std::string instanceId,
        std::function <void(std::string)> onExit
    );
    ~Debugger();

    /**
     *  Allow copying.
     */
    Debugger& operator=(Debugger const& other) = delete;
    Debugger(Debugger const& other) = delete;

    Debugger& operator=(Debugger&&) = default;
    Debugger(Debugger&&) = default;

    /**
     *  Returns the used run config.
     */
    RunConfig runConfig() const;

    void command(DebuggerInterface::MiCommand const& command) const;
    void command(std::string const& command) const;

    /**
     *  Returns the name of the run config used
     */
    std::string_view runConfigName() const;

    /**
     *  Executes the debugger process
     */
    void start(std::optional <std::unordered_map <std::string, std::string>> const& env);

    void stop();

public: // Debugger Interface Implementations
    void onRawData(std::string const& raw) override;
    void onLogStream(std::string const& message) override;
    void onConsoleStream(std::string const& message) override;
    void onParserError(std::string const& message) override;
    void onStdErr(std::string const& text) override;
    void onResult(DebuggerInterface::ResultRecord const& result) override;
    void onExec(DebuggerInterface::AsyncRecord const& record) override;
    void onStatus(DebuggerInterface::AsyncRecord const& record) override;
    void onNotify(DebuggerInterface::AsyncRecord const& record) override;
    void onPartialRemain(std::string const& remain, std::string const& subject) override;

private:
    void relayMessage(json j);
    void relayAsyncRecord(std::string const& type, DebuggerInterface::AsyncRecord const& record);

private:
    Streaming::StreamerBase* streamer_;
    std::string remoteAddress_;
    int controlId_;
    RunConfig runConfig_;
    std::string instanceId_;
    std::mutex relayLock_;

    // shared_ptr should in theory be unique_ptr, but
    // this class has to be copyable. Also could implement that for unique_ptr, but having share semantics here is irrelevant.
    // maybe fix later
    std::shared_ptr <DebuggerInterface::Debugger> debugInterface_;
    std::unique_ptr<std::jthread> debuggerWatchdog_;
    std::function <void(std::string)> onExit_;
};
