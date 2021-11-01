#pragma once

#include <backend/plugin_system/isolate.hpp>
#include <backend/filesystem/filesystem.hpp>

#include <v8.h>

#include <memory>

namespace PluginSystem
{

    class Script
    {
    public:
        Script(Isolate& isolate, std::string const& script);
        ~Script();
        Script(Script&&);
        Script& operator=(Script&&);

        v8::Local<v8::Value> run();

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };

}