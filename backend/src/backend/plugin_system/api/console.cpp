#include <backend/plugin_system/api/console.hpp>

#include <v8pp/convert.hpp>

#include <iostream>

namespace PluginSystem::PluginApi::Console
{

void log(v8::FunctionCallbackInfo<v8::Value> const& args)
{    
    v8::HandleScope handle_scope(args.GetIsolate());

    auto context = args.GetIsolate()->GetCurrentContext();
    v8::Local<v8::Object> JSON = context->
        Global()->Get(context, v8::String::NewFromUtf8(args.GetIsolate(), "JSON").ToLocalChecked()).ToLocalChecked()->ToObject(context).ToLocalChecked();
    v8::Local<v8::Function> stringify = JSON->Get(context, v8::String::NewFromUtf8(args.GetIsolate(), "stringify").ToLocalChecked()).ToLocalChecked().As<v8::Function>();

    for (int i = 0; i < args.Length(); ++i)
    {
        if (i > 0) 
            std::cout << ' ';
        //v8::String::Utf8Value str(args[i]);
        v8::Local<v8::Value> arg = args[i];
        auto value = stringify->Call(context, JSON, 1, &arg).ToLocalChecked();
        if (value->IsUndefined())
            std::cout << "undefined";
        else if (value->IsNull())
            std::cout << "null";
        else if (value->IsString())
            std::cout << v8pp::from_v8<std::string>(args.GetIsolate(), value.As<v8::String>());
    }
    std::cout << std::endl;
}

}