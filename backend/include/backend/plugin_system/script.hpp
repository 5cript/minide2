#pragma once

#include <v8.h>

namespace PluginSystem
{
    class Script
    {
    public:
        struct CreationParameters
        {
            v8::Isolate* isolate;
            std::string fileName;
            std::string script;
        };

        Script(CreationParameters&& params);
        ~Script();

        v8::Local<v8::Value> run();
        v8::Local<v8::Context>& context();

    private:
        v8::Local<v8::Context> scriptContext;
        v8::Context::Scope contextScope;
        v8::ScriptOrigin origin;
        v8::ScriptCompiler::Source source;
        v8::Local<v8::Script> script;
    };
}
