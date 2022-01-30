#pragma once

#include <v8.h>

#include <iostream>

namespace PluginSystem::PluginApi::Console
{

    std::ostream& print(v8::Local<v8::Context> context, v8::Local<v8::Value> value);
    void log(v8::FunctionCallbackInfo<v8::Value> const& args);

}