#pragma once

#include <v8.h>

#include <v8wrap/array.hpp>
#include <v8wrap/object.hpp>

#include <v8pp/module.hpp>
#include <v8pp/class.hpp>

#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <iostream>

namespace PluginSystem::PluginApi
{
    struct Toolbar
    {
        std::string pluginType = "Toolbar";

        Toolbar()
        {}
        v8::Local<v8::Value> makeMenu(v8::FunctionCallbackInfo<v8::Value> const& args);
        v8::Local<v8::Value> makeSplitter(v8::FunctionCallbackInfo<v8::Value> const& args);
        v8::Local<v8::Value> makeIconButton(v8::FunctionCallbackInfo<v8::Value> const& args);
    };

    inline void makeToolbarClass(v8::Local<v8::Context> context, v8pp::jsmodule& mod)
    {
        v8pp::class_<Toolbar> toolbar(context->GetIsolate());
        toolbar.ctor<>()
            .set("pluginType", &Toolbar::pluginType)
            .set("makeMenu", &Toolbar::makeMenu)
            .set("makeSplitter", &Toolbar::makeSplitter)
            .set("makeIconButton", &Toolbar::makeIconButton);
        mod.set("Toolbar", toolbar);
    }
}