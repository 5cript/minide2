#pragma once

#include <backend/plugin_system/contextualizer.hpp>
#include <backend/plugin_system/plugin_implementation.hpp>
#include <backend/server/stream/subscriber.hpp>

#include <v8.h>

#include <v8wrap/array.hpp>
#include <v8wrap/object.hpp>

#include <v8pp/module.hpp>
#include <v8pp/class.hpp>

#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <iostream>

namespace Backend::PluginSystem::PluginApi
{
    class Toolbar
        : public PluginImplementation
        , public Contextualized<Toolbar>
        , public Server::Stream::Subscriber
    {
      public:
        static const std::string pluginType;

        Toolbar();
        ~Toolbar();

        void initialize(v8::Local<v8::Context> context);

      public:
        v8::Local<v8::Value> makeMenu(v8::FunctionCallbackInfo<v8::Value> const& args);
        v8::Local<v8::Value> makeSplitter(v8::FunctionCallbackInfo<v8::Value> const& args);
        v8::Local<v8::Value> makeIconButton(v8::FunctionCallbackInfo<v8::Value> const& args);

      private:
        struct ToolbarAction
        {
            Toolbar* toolbar;
            v8wrap::Object<>* decorated;
            std::string path;
            std::function<void(v8::Local<v8::Context> context, v8wrap::Object<>& plugin, json const& j)> callback;
        };
        void registerActionHandler(ToolbarAction options);

      private:
        struct Implementation;
        std::unique_ptr<Implementation> impl_;
    };

    template <typename OnConstructionCallbackT>
    inline void makeToolbarClass(
        v8::Local<v8::Context> context,
        v8pp::module& mod,
        OnConstructionCallbackT&& onConstructionCallback)
    {
        v8pp::class_<Toolbar> toolbar(context->GetIsolate());

        Contextualizer<Toolbar>::Constructor<>::bindConstructor(
            toolbar, std::forward<OnConstructionCallbackT>(onConstructionCallback));

        toolbar.const_("pluginType", Toolbar::pluginType)
            .function("makeMenu", &Toolbar::makeMenu)
            .function("makeSplitter", &Toolbar::makeSplitter)
            .function("makeIconButton", &Toolbar::makeIconButton);
        mod.class_("Toolbar", toolbar);
    }
}