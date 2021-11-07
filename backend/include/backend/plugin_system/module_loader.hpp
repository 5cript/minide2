#pragma once

#include "module.hpp"
#include "api/plugin_self.hpp"
#include <v8.h>

#include <vector>
#include <memory>
#include <string>

namespace PluginSystem
{
    class ModuleLoader
    {
    public:
        v8::MaybeLocal<v8::Module> load(v8::Local<v8::Context> context, std::string const& path);

    private:
        v8::MaybeLocal<v8::Module> loadSynthetic(v8::Local<v8::Context> context, std::string const& path, v8::Local<v8::Value> value);
    
    private:
        std::vector<std::shared_ptr <Module>> loadedModules_;
    };
}