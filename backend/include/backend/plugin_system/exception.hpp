#pragma once

#include <v8.h>

namespace PluginSystem
{
    class JavaScriptException : public std::runtime_error
    {
    public:
        JavaScriptException(std::string const& str);
    };

    void rethrowException(v8::Isolate* isolate, v8::TryCatch& jsException);
}