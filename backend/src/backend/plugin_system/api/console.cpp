#include <backend/plugin_system/api/console.hpp>

#include <v8pp/convert.hpp>

#include <iostream>

namespace Backend::PluginSystem::PluginApi::Console
{
    //#####################################################################################################################
    void log(v8::FunctionCallbackInfo<v8::Value> const& args)
    {
        v8::HandleScope handle_scope(args.GetIsolate());

        auto context = args.GetIsolate()->GetCurrentContext();

        for (int i = 0; i < args.Length(); ++i)
        {
            v8::Local<v8::Value> arg = args[i];
            if (i > 0)
                std::cout << ' ';
            print(context, arg);
        }
        std::cout << std::endl;
    }
    //---------------------------------------------------------------------------------------------------------------------
    std::ostream& print(v8::Local<v8::Context> context, v8::Local<v8::Value> value)
    {
        v8::HandleScope handle_scope(context->GetIsolate());
        v8::Local<v8::Object> JSON =
            context->Global()
                ->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "JSON").ToLocalChecked())
                .ToLocalChecked()
                ->ToObject(context)
                .ToLocalChecked();
        v8::Local<v8::Function> stringify =
            JSON->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "stringify").ToLocalChecked())
                .ToLocalChecked()
                .As<v8::Function>();

        auto jsonStringValue = stringify->Call(context, JSON, 1, &value).ToLocalChecked();
        if (jsonStringValue->IsUndefined())
            std::cout << "undefined";
        else if (jsonStringValue->IsNull())
            std::cout << "null";
        else if (jsonStringValue->IsString())
            std::cout << v8pp::from_v8<std::string>(context->GetIsolate(), jsonStringValue.As<v8::String>());
        return std::cout;
    }
    //#####################################################################################################################
}