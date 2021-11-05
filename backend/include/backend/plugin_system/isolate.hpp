#pragma once

#include <backend/filesystem/filesystem.hpp>

#include <v8.h>

#include <memory>

namespace PluginSystem
{
    class Isolate
    {
    public:
        Isolate();
        ~Isolate();

        Isolate(Isolate&&);
        Isolate& operator=(Isolate&&);

        v8::Isolate* handle();
        operator v8::Isolate*();
        v8::Isolate* operator->()
        {
            return handle();
        }

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}