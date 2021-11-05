#include <backend/plugin_system/plugin.hpp>

#include <backend/filesystem/home_directory.hpp>
#include <backend/plugin_system/isolate.hpp>
#include <backend/plugin_system/script.hpp>
#include <backend/plugin_system/api/console.hpp>
#include <backend/plugin_system/api/plugin_data.hpp>
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
        Script mainScript;
    };
//---------------------------------------------------------------------------------------------------------------------
    Plugin::Implementation::Implementation(std::string const& pluginName, Api::AllApis const& allApis)
        : name{pluginName}
        , pluginDirectory{inHomeDirectory() / Plugin::pluginsDir / pluginName}
        , pluginDataDirectory{inHomeDirectory() / Plugin::pluginDataDir / pluginName}
        , apis{allApis}
        , isolate{}
        , mainScript{[this](){
            std::ifstream reader{pluginDirectory / pluginMainFile, std::ios_base::binary};
            if (!reader.good())
                throw std::runtime_error("Cannot open plugin main.js file.");

            reader.seekg(0, std::ios_base::end);
            std::string content(reader.tellg(), '\0');
            reader.seekg(0, std::ios_base::beg);
            reader.read(content.data(), content.size());
            return Script{isolate, content};
        }()}
    {
    }
//#####################################################################################################################
    Plugin::Plugin(std::string const& pluginName, Api::AllApis const& allApis)
        : impl_{std::make_unique<Implementation>(pluginName, allApis)}
    {
        exposeScriptApi();
    }
//---------------------------------------------------------------------------------------------------------------------
    void Plugin::run() const
    {
        impl_->mainScript.run();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string Plugin::name() const
    {
        return impl_->name;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Plugin::exposeScriptApi()
    {
        // plugin data
        auto context = impl_->isolate->GetCurrentContext();
        v8pp::jsmodule pluginDataModule(impl_->isolate);

        v8pp::class_<PluginApi::PluginData> pluginData(impl_->isolate);
        pluginData
            .ctor<>()
            .set("var", &PluginApi::PluginData::test)
        ;

        pluginDataModule.set("data", pluginData);
        (void)context->Global()->Set(
            context,
            v8::String::NewFromUtf8(impl_->isolate, "pluginData").ToLocalChecked(), 
            pluginDataModule.new_instance()
        );

        // console
        v8pp::jsmodule console(impl_->isolate);
        console.set("log", &PluginApi::Console::log);
        v8::Local<v8::Object> global = context->Global()->GetPrototype().As<v8::Object>();
        global->Set(context, v8pp::to_v8(impl_->isolate, "console"), console.new_instance());
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