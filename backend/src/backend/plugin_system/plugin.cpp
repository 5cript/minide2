#include <backend/plugin_system/plugin.hpp>

#include <backend/log.hpp>
#include <backend/filesystem/home_directory.hpp>
#include <backend/filesystem/load_file.hpp>
#include <backend/plugin_system/isolate.hpp>
#include <backend/plugin_system/module.hpp>
#include <backend/plugin_system/script.hpp>
#include <backend/plugin_system/module_loader.hpp>
#include <backend/plugin_system/api/console.hpp>
#include <v8pp/class.hpp>
#include <v8pp/module.hpp>

#include <filesystem>

namespace PluginSystem
{
//#####################################################################################################################
    struct Plugin::Implementation
    {
        Implementation(std::string const& pluginName, Api::AllApis const& allApis);

        std::string name;
        sfs::path pluginDirectory;
        sfs::path pluginDataDirectory;
        Api::AllApis apis;
        Isolate isolate;
        std::shared_ptr<Script> mainScript;
        std::shared_ptr<Module> mainModule;
        ModuleLoader moduleLoader;
    };
//---------------------------------------------------------------------------------------------------------------------
    Plugin::Implementation::Implementation(std::string const& pluginName, Api::AllApis const& allApis)
        : name{pluginName}
        , pluginDirectory{inHomeDirectory() / Plugin::pluginsDir / pluginName}
        , pluginDataDirectory{inHomeDirectory() / Plugin::pluginDataDir / pluginName}
        , apis{allApis}
        , isolate{}
        , mainScript{std::make_shared<Script>(Script::CreationParameters{
            .isolate = isolate,
            .fileName = pluginName + "/main.js",
            .script = "",
        })}
        , mainModule{std::make_shared<Module>(Module::CreationParameters{
            .context = mainScript->context(), 
            .fileName = pluginName + "/main.js",
            .script = Filesystem::loadFile(pluginDirectory / pluginMainFile),
            .onModuleLoad = [this](v8::Local<v8::Context> ctx, std::string const& path){
                return moduleLoader.load(ctx, path);
            }
        })}
    {
    }
//#####################################################################################################################
    Plugin::Plugin(std::string const& pluginName, Api::AllApis const& allApis)
        : impl_{std::make_unique<Implementation>(pluginName, allApis)}
    {
        exposeGlobals();
    }
//---------------------------------------------------------------------------------------------------------------------
    void Plugin::run() const
    {
        const auto promise = impl_->mainModule->evaluate().As<v8::Promise>();
        std::cout << promise->State() << "\n";
        if (promise->State() == v8::Promise::PromiseState::kRejected)
        {
            promise->Catch(impl_->mainScript->context(), v8::Function::New(
                impl_->mainScript->context(), 
                [](auto...) {
                    //PluginApi::Console::log(args);
                    std::cout << "caught\n";
                }).ToLocalChecked()
            );
        }
        promise->Then(impl_->mainScript->context(), v8::Function::New(
            impl_->mainScript->context(), 
            [](auto...) {
                //PluginApi::Console::log(args);
            }).ToLocalChecked()
        );
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string Plugin::name() const
    {
        return impl_->name;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Plugin::exposeGlobals()
    {
        // plugin data
        auto context = impl_->isolate->GetCurrentContext();

        // v8::Local<v8::Module> pluginSelfApi = v8::Module::CreateSyntheticModule(
        //     impl_->isolate, 
        //     v8pp::to_v8(impl_->isolate, "minide_plugin"),
        //     {v8pp::to_v8(impl_->isolate, "minide_plugin")},
        //     +[](v8::Local<v8::Context> context, v8::Local<v8::Module> mod) -> v8::MaybeLocal<v8::Value>{        
        //         /*
        //         v8pp::jsmodule pluginModule(context->GetIsolate());
        //         v8pp::class_<PluginApi::PluginData> pluginData(context->GetIsolate());
        //         pluginData
        //             .ctor<>()
        //             .set("var", &PluginApi::PluginData::test)
        //         ;
        //         pluginModule.set("data", pluginData);
        //         */
        //         mod->SetSyntheticModuleExport(v8pp::to_v8(context->GetIsolate(), "minide_plugin"), v8pp::to_v8(context->GetIsolate(), 1));
        //         return v8pp::to_v8(context->GetIsolate(), 2);
        //     }
        // );
        // v8::Global<v8::Module> test{impl_->isolate, pluginSelfApi};

        // console
        v8pp::jsmodule console(impl_->isolate);
        console.set("log", &PluginApi::Console::log);
        v8::Local<v8::Object> global = context->Global()->GetPrototype().As<v8::Object>();
        (void)global->Set(context, v8pp::to_v8(impl_->isolate, "console"), console.new_instance());
        /*
        (void)context->Global()->Set(
            context,
            v8::String::NewFromUtf8(impl_->isolate, "console").ToLocalChecked(), 
            pluginDataModule.new_instance()
        );
        */
    }
//---------------------------------------------------------------------------------------------------------------------
    Plugin::~Plugin() = default;
//---------------------------------------------------------------------------------------------------------------------
    Plugin::Plugin(Plugin&&) = default;
//---------------------------------------------------------------------------------------------------------------------
    Plugin& Plugin::operator=(Plugin&&) = default;
//#####################################################################################################################
}