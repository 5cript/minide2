#include <backend/plugin_system/script.hpp>

namespace PluginSystem
{
//#####################################################################################################################
    struct Script::Implementation
    {
        v8::Local<v8::Context> context;
        v8::Context::Scope contextScope;
        v8::Local<v8::String> source;
        v8::Local<v8::Script> script;

        Implementation(Isolate& isolate, std::string const& scriptData);
    };
//---------------------------------------------------------------------------------------------------------------------
    Script::Implementation::Implementation(Isolate& isolate, std::string const& scriptData)
        : context{v8::Context::New(isolate)}
        , contextScope{context}
        , source{v8::String::NewFromUtf8(isolate, scriptData.c_str(), v8::NewStringType::kNormal).ToLocalChecked()}
        , script{v8::Script::Compile(context, source).ToLocalChecked()}
    {
    }
//#####################################################################################################################
    Script::Script(Isolate& isolate, std::string const& scriptData)
        : impl_{std::make_unique<Implementation>(isolate, scriptData)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    Script::~Script() = default;
//---------------------------------------------------------------------------------------------------------------------
    Script::Script(Script&&) = default;
//---------------------------------------------------------------------------------------------------------------------
    Script& Script::operator=(Script&&) = default;
//---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Value> Script::run()
    {
        return impl_->script->Run(impl_->context).ToLocalChecked();
    }
//#####################################################################################################################
}