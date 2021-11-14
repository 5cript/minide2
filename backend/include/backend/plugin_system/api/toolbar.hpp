#pragma once

#include <v8.h>

#include <v8pp/module.hpp>
#include <v8pp/class.hpp>

namespace PluginSystem::PluginApi
{
    struct Toolbar
    {
        std::string pluginType = "Toolbar";

        Toolbar() {}
        int makeMenu(v8::Local<v8::Array> menuEntries) 
        {
            return 0;
        }
        int makeSplitter()
        {
            return 0;
        }
        int makeIconButton(v8::Local<v8::Object> paramObject)
        {
            return 0;
        }
    };

    inline void makeToolbarClass(v8::Local<v8::Context> context, v8pp::jsmodule& mod)
    {
        v8pp::class_<Toolbar> toolbar(context->GetIsolate());
        toolbar
            .ctor<>()
            .set("pluginType", &Toolbar::pluginType)
            .set("makeMenu", &Toolbar::makeMenu)
            .set("makeSplitter", &Toolbar::makeSplitter)
            .set("makeIconButton", &Toolbar::makeIconButton)
        ;
        mod.set("Toolbar", toolbar);
    }
}