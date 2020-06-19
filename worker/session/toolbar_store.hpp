#pragma once

#include "../toolbars/scripted_toolbar.hpp"

class ToolbarStore
{
public:
    // mild crime with shared_ptr:
    std::vector <std::shared_ptr<Toolbars::ScriptedToolbar>> scriptedToolbars;

    void reset();
};
