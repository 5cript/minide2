#pragma once

#include <string>

class Debugger
{
public:
    Debugger();
    void start(std::string const& debuggerExecuteable);
};
