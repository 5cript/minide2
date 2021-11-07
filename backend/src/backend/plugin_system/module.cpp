#include <backend/plugin_system/module.hpp>
#include <backend/plugin_system/exception.hpp>
#include <backend/plugin_system/core.hpp>
#include <backend/log.hpp>

#include <v8pp/convert.hpp>

#include <iostream>

namespace PluginSystem
{
//#####################################################################################################################
    struct Module::Implementation
    {
        v8::Local<v8::Context> context;
        v8::ScriptOrigin origin;
        v8::ScriptCompiler::Source source;
        v8::Context::Scope contextScope;
        Module::CreationParameters::onModuleLoadFunction moduleLoader;
        // synthetic only:
        v8::Local<v8::Value> exported;
        v8::Local<v8::Module> mod;
        std::once_flag initOnce;
        bool synthetic;

        Implementation(Module::CreationParameters&& params);
        Implementation(Module::SyntheticCreationParameters&& params);
    };
//---------------------------------------------------------------------------------------------------------------------
    Module::Implementation::Implementation(Module::CreationParameters&& params)
        : context{params.context}
        , origin{
            v8::String::NewFromUtf8(params.context->GetIsolate(), params.fileName.c_str()).ToLocalChecked(),      // specifier
            v8::Integer::New(params.context->GetIsolate(), 0),             // line offset
            v8::Integer::New(params.context->GetIsolate(), 0),             // column offset
            v8::False(params.context->GetIsolate()),                       // is cross origin
            v8::Local<v8::Integer>(),                     // script id
            v8::Local<v8::Value>(),                       // source map URL
            v8::False(params.context->GetIsolate()),                       // is opaque
            v8::False(params.context->GetIsolate()),                       // is WASM
            v8::True(params.context->GetIsolate())                         // is ES6 module
        }
        , source{
            v8::String::NewFromUtf8(params.context->GetIsolate(), params.script.c_str(), v8::NewStringType::kNormal).ToLocalChecked(), 
            origin
        }
        , contextScope{context}
        , moduleLoader{std::move(params.onModuleLoad)}
        , exported{}
        , mod{[this]() {
            v8::TryCatch exc(context->GetIsolate());
            v8::Local<v8::Module> mod_;
            if (!v8::ScriptCompiler::CompileModule(context->GetIsolate(), &source).ToLocal(&mod_))
            {
                if (exc.HasCaught())
                    rethrowException(context->GetIsolate(), exc);
                else
                    throw std::runtime_error("Could not compile module.");
            }
            return mod_;
        }()}
        , initOnce{}
        , synthetic{false}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    Module::Implementation::Implementation(Module::SyntheticCreationParameters&& params)
        : context{params.context}
        , origin{
            v8::String::NewFromUtf8(params.context->GetIsolate(), params.fileName.c_str()).ToLocalChecked(),      // specifier
            v8::Integer::New(params.context->GetIsolate(), 0),             // line offset
            v8::Integer::New(params.context->GetIsolate(), 0),             // column offset
            v8::False(params.context->GetIsolate()),                       // is cross origin
            v8::Local<v8::Integer>(),                     // script id
            v8::Local<v8::Value>(),                       // source map URL
            v8::False(params.context->GetIsolate()),                       // is opaque
            v8::False(params.context->GetIsolate()),                       // is WASM
            v8::True(params.context->GetIsolate())                         // is ES6 module
        }
        , source{
            v8::String::NewFromUtf8(params.context->GetIsolate(), "", v8::NewStringType::kNormal).ToLocalChecked(), 
            origin
        }
        , contextScope{context}
        , moduleLoader{std::move(params.onModuleLoad)}
        , exported{params.exported}
        , mod{[this, &params]() {
            v8::TryCatch exc(context->GetIsolate());
            v8::Local<v8::Module> mod_ = v8::Module::CreateSyntheticModule(
                context->GetIsolate(),
                v8pp::to_v8(context->GetIsolate(), params.fileName),
                {v8pp::to_v8(context->GetIsolate(), params.fileName)},
                [](v8::Local<v8::Context> ctx, v8::Local<v8::Module> m) -> v8::MaybeLocal<v8::Value>
                {
                    auto wrap = findModule(m);
                    if (wrap)
                    {
                        LOG() << "SetSyntheticModuleExport\n";
                        auto maybe = m->SetSyntheticModuleExport(
                            ctx->GetIsolate(), 
                            v8::String::NewFromUtf8(ctx->GetIsolate(), "default").ToLocalChecked(), 
                            wrap->impl_->exported
                        );
                        if (maybe.IsNothing() || maybe.FromJust() == false)
                            return v8::False(ctx->GetIsolate());
                        return v8::True(ctx->GetIsolate());
                    }
                    LOG() << "Could not find module when initializing synthetic module\n";
                    return v8::False(ctx->GetIsolate());
                }
            );
            return mod_;
        }()}
        , synthetic{true}
    {
    }
//#####################################################################################################################
    Module::Module(CreationParameters&& params)
        : impl_{std::make_unique<Implementation>(std::move(params))}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    Module::Module(SyntheticCreationParameters&& params)
        : impl_{std::make_unique<Implementation>(std::move(params))}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void Module::setup()
    {
        std::call_once(impl_->initOnce, [this]()
        {
            registerModule(impl_->mod->GetIdentityHash(), weak_from_this());
            if (!impl_->synthetic)
            {
                // You can resolve import requests ahead of time (useful for async)
                // for (int i = 0; i < impl_->mod->GetModuleRequestsLength(); i++) {
                //     v8::Local<v8::String> specifier = mod->GetModuleRequest(i); // "some thing"
                //     std::cout << "Module request preinfo: " << v8pp::from_v8<std::string>(params.context->GetIsolate(), specifier) << "\n";
                // }

                // or you can resolve them sync in the InstantiateModule callback
                v8::TryCatch exc(impl_->context->GetIsolate());
                (void)impl_->mod->InstantiateModule(impl_->context, [](
                    v8::Local<v8::Context> ctx, // "main.mjs"
                    v8::Local<v8::String> specifier, // "some thing"
                    v8::Local<v8::Module> referrer
                ) -> v8::MaybeLocal<v8::Module> 
                {
                    LOG() << "Loading module of id " << referrer->GetIdentityHash() << " in module store\n";
                    auto m = findModule(referrer);
                    if (!m)
                    {
                        LOG() << "Could not find module of id " << referrer->GetIdentityHash() << " in module store\n";
                        return v8::MaybeLocal<v8::Module>{};
                    }
            
                    v8::TryCatch exc(ctx->GetIsolate());
                    auto maybeMod = m->impl_->moduleLoader(ctx, v8pp::from_v8<std::string>(ctx->GetIsolate(), specifier));
                    if (!maybeMod.IsEmpty())
                        return maybeMod.ToLocalChecked();
                    else
                    {
                        LOG() << "Loaded module is empty\n";
                        return {};
                    }
                });
                if (exc.HasCaught() && !exc.HasTerminated()) 
                    rethrowException(impl_->context->GetIsolate(), exc);
                
                // setting this callback enables dynamic import
                // isolate->SetHostImportModuleDynamicallyCallback([](
                //     v8::Local<v8::Context> context,
                //     v8::Local<v8::ScriptOrModule> referrer,
                //     v8::Local<v8::String> specifier
                // ) -> v8::MaybeLocal<v8::Promise> 
                // {
                //     v8::Isolate* isolate = context->GetIsolate();
                //     v8::EscapableHandleScope handle_scope(isolate);
                    
                //     // Local<Value> result;
                //     // if (maybe_result.ToLocal(&result)) 
                //     // {
                //     //     return handle_scope.Escape(result.As<Promise>());
                //     // }
                //     //return v8::MaybeLocal<v8::Promise>();
                //     v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context).ToLocalChecked();
                //     resolver->Resolve(context, v8::Local<v8::Module>{});
                //     return resolver->GetPromise();
                // });

                /*
                // setting this callback enables import.meta
                isolate->SetHostInitializeImportMetaObjectCallback([](Local<Context> context,
                                                                    Local<Module> module,
                                                                    Local<Object> meta) {
                // meta->Set(key, value); you could set import.meta.url here
                });*/
            }
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Context>& Module::context()
    {
        return impl_->context;
    }
//---------------------------------------------------------------------------------------------------------------------
    Module::~Module()
    {
        unregisterModule(impl_->mod->GetIdentityHash(), weak_from_this());
    }
//---------------------------------------------------------------------------------------------------------------------
    Module::Module(Module&&) = default;
//---------------------------------------------------------------------------------------------------------------------
    Module& Module::operator=(Module&&) = default;
//---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Module> Module::getModule()
    {
        return impl_->mod;
    }
//---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Value> Module::evaluate()
    {
        setup();
        LOG() << "Evaluating module with id " << impl_->mod->GetIdentityHash() << ".\n";

       v8::EscapableHandleScope handle_scope(impl_->context->GetIsolate());
       
        v8::TryCatch exc(impl_->context->GetIsolate());
        v8::Local<v8::Value> result;
        const auto maybeEval = impl_->mod->Evaluate(impl_->context);
        if (!maybeEval.IsEmpty() && maybeEval.ToLocal(&result)) {
            v8::String::Utf8Value utf8(impl_->context->GetIsolate(), result);
        } else {
            if (exc.HasCaught())
                rethrowException(impl_->context->GetIsolate(), exc);
            else
                throw std::runtime_error("Unknown error in module.");
        }
        return result;
    }
//#####################################################################################################################
}