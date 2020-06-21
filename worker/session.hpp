#pragma once

#include "workspace/workspace.hpp"
#include "session/toolbar_store.hpp"

#include <attender/attender/session/session.hpp>
#include <string>
#include <functional>
#include <mutex>

/**
 *  Keep this class as clean as possible. little functionality, more just plain data.
 */
class Session : public attender::session
{
public:
    int dataId;
    int controlId;
    WorkspaceInfo workspace;
    std::shared_ptr <std::mutex> sessionLock;

    ToolbarStore toolbarStore;

    void dump() const;

public:
    Session(std::string id = "");
    ~Session() = default;
};
