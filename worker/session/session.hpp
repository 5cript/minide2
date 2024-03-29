#pragma once

#include "session_fwd.hpp"
#include "../workspace/workspace.hpp"
#include "toolbar_store.hpp"
#include "../debugger/debugger.hpp"

#include <attender/session/session.hpp>
#include <string>
#include <functional>
#include <mutex>
#include <unordered_map>

/**
 *  Keep this class as clean as possible. little functionality, more just plain data.
 */
class Session : public attender::session
{
public:
    int dataId;
    int controlId;
    WorkspaceInfo workspace;
    std::string remoteAddress;
    std::string terminalEnvironment;
    std::unordered_map <std::string, std::shared_ptr <Debugger>> debuggerInstances;

    ToolbarStore toolbarStore;

    void dump() const;

public:
    Session(std::string id = "");
    ~Session() = default;
};
