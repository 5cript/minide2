#pragma once

#include <v8.h>

#include <v8pp/module.hpp>
#include <v8pp/class.hpp>

#include <iostream>

namespace Backend::PluginSystem::PluginApi
{
    struct EditorControl
    {
        EditorControl()
        {}
        void save()
        {
            std::cout << "save!\n";
        }
        void saveAll()
        {
            std::cout << "saveAll!\n";
        }
    };

    inline void makeEditorControlClass(v8::Local<v8::Context> context, v8pp::module& mod)
    {
        v8pp::class_<EditorControl> EditorControl(context->GetIsolate());
        EditorControl.ctor<>().function("save", &EditorControl::save).function("saveAll", &EditorControl::saveAll);
        mod.class_("EditorControl", EditorControl);
    }
}