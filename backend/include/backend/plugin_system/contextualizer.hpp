#pragma once

#include <v8pp/class.hpp>

#include <tuple>

namespace Backend::PluginSystem
{
    template <typename T>
    class Contextualized
    {
      public:
        template <typename OnConstructionCallbackT>
        void contextualize(OnConstructionCallbackT&& onConstructionCallback)
        {
            onConstructionCallback(static_cast<T*>(this));
        }
    };

    template <typename T>
    class Contextualizer
    {
      public:
        template <typename... ConstructionArgs>
        struct Constructor
        {
            template <typename... InitializeArguments>
            static void bindConstructor(v8pp::class_<T>& leClass, InitializeArguments&&... initArgs)
            {
                leClass.template ctor<ConstructionArgs...>(
                    [initArgs = std::tuple<InitializeArguments...>{std::forward<InitializeArguments>(initArgs)...}](
                        v8::FunctionCallbackInfo<v8::Value> const& args) ->
                    typename v8pp::class_<T>::object_pointer_type {
                        typename v8pp::class_<T>::object_pointer_type object =
                            v8pp::detail::call_from_v8<v8pp::raw_ptr_traits>(
                                v8pp::raw_ptr_traits::template create<T, ConstructionArgs...>, args);
                        args.GetIsolate()->AdjustAmountOfExternalAllocatedMemory(
                            static_cast<int64_t>(v8pp::raw_ptr_traits::object_size(object)));
                        std::apply(
                            [&object]<typename... Ts>(Ts && ... args) { object->contextualize(std::move(args)...); },
                            std::move(initArgs));
                        return object;
                    });
            }
        };
    };
}