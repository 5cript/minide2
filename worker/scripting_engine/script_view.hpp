#pragma once

#include "script.hpp"

#include <string_view>

namespace MinIDE::Scripting
{
    class ScriptView
    {
    public:
        ScriptView(Script const* script);

        ScriptView(ScriptView const&) = default;
        ScriptView(ScriptView&&) = default;
        ScriptView& operator=(ScriptView const&) = default;
        ScriptView& operator=(ScriptView&&) = default;

    protected:
        std::string_view script() const;

    private:
        Script const* script_;
    };
}
