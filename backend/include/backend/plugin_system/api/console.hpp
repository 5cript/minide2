#pragma once

#include <v8.h>

namespace PluginSystem::PluginApi::Console
{

void log(v8::FunctionCallbackInfo<v8::Value> const& args);

}