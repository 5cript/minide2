#pragma once

#include <backend/filesystem/filesystem.hpp>

#include <v8.h>

#include <v8pp/module.hpp>
#include <v8pp/class.hpp>

#include <iostream>

namespace Backend::PluginSystem::PluginApi
{
    class ResourceAccessor
    {
      public:
        ResourceAccessor(sfs::path const& resourceDirectory);
        std::string loadPng(std::string const& fileName);

      private:
        sfs::path resourceDirectory_;
    };

    inline void makeResourceAccessorClass(
        v8::Local<v8::Context> context,
        v8pp::module& mod,
        std::filesystem::path const& resourceDirectory)
    {
        v8pp::class_<ResourceAccessor> resourceAccessor(context->GetIsolate());
        resourceAccessor
            .ctor<>([&](v8::FunctionCallbackInfo<v8::Value> const&) {
                return new ResourceAccessor(resourceDirectory);
            })
            .function("loadPng", &ResourceAccessor::loadPng);

        mod.class_("ResourceAccessor", resourceAccessor);
    }
}