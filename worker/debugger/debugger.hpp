#pragma once

#include <debugger-interface/debugger.hpp>
#include <debugger-interface/debugger_interface.hpp>

#include "../workspace/run_config.hpp"

#include <string>
#include <memory>
#include <optional>
#include <map>
#include <string>

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
        RunConfig::Contents::Configuration const& usedConfig = {},
        std::string instanceId = {}
    );
    ~Debugger();

    /**
     *  Allow copying.
     */
    Debugger& operator=(Debugger const& other) = default;
    Debugger(Debugger const& other) = default;

    Debugger& operator=(Debugger&&) = default;
    Debugger(Debugger&&) = default;

    /**
     *  Returns the used run config.
     */
    RunConfig::Contents::Configuration runConfig() const;

    /**
     *  Returns the name of the run config used
     */
    std::string_view runConfigName() const;

    /**
     *  Executes the debugger process
     */
    void start(std::optional <std::unordered_map <std::string, std::string>> const& env);

public: // Debugger Interface Implementations
    void onRawData(std::string const& raw) override;
    void onLogStream(std::string const& message) override;
    void onConsoleStream(std::string const& message) override;

private:
    RunConfig::Contents::Configuration runConfig_;
    std::string instanceId_;

    // shared_ptr should in theory be unique_ptr, but
    // this class has to be copyable. Also could implement that for unique_ptr, but having share semantics here is irrelevant.
    // maybe fix later
    std::shared_ptr <DebuggerInterface::Debugger> debugInterface_;
};
