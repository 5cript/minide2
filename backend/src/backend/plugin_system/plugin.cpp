#include <backend/plugin_system/plugin.hpp>

#include <backend/log.hpp>
#include <backend/filesystem/home_directory.hpp>
#include <backend/filesystem/load_file.hpp>
#include <v8wrap/isolate.hpp>
#include <v8wrap/module.hpp>
#include <v8wrap/script.hpp>
#include <v8wrap/value.hpp>
#include <v8wrap/exception.hpp>
#include <v8wrap/object.hpp>
#include <v8wrap/module_loader.hpp>

#include <backend/plugin_system/toolbar_plugin.hpp>
#include <backend/plugin_system/api/console.hpp>
#include <backend/plugin_system/api/plugin_self.hpp>
#include <backend/plugin_system/api/toolbar.hpp>
#include <backend/plugin_system/api/editor_control.hpp>
#include <backend/plugin_system/api/resource_accessor.hpp>

#include <v8pp/class.hpp>
#include <v8pp/module.hpp>

#include <filesystem>
#include <iostream>

using namespace std::string_literals;

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
        v8wrap::Isolate isolate;
        std::shared_ptr<v8wrap::Script> mainScript;
        std::shared_ptr<v8wrap::Module> mainModule;
        v8wrap::ModuleLoader moduleLoader;
        std::unique_ptr<PluginImplementation> pluginClass;

        void runModule(v8::Local<v8::Value> defaultExport, Plugin const* plugin);
    };
    //---------------------------------------------------------------------------------------------------------------------
    Plugin::Implementation::Implementation(std::string const& pluginName, Api::AllApis const& allApis)
        : name{pluginName}
        , pluginDirectory{inHomeDirectory() / Plugin::pluginsDir / pluginName}
        , pluginDataDirectory{inHomeDirectory() / Plugin::pluginDataDir / pluginName}
        , apis{allApis}
        , isolate{}
        , mainScript{std::make_shared<v8wrap::Script>(v8wrap::Script::CreationParameters{
              .isolate = isolate,
              .fileName = pluginName + "/main.js",
              .script = "",
          })}
        , mainModule{std::make_shared<v8wrap::Module>(v8wrap::Module::CreationParameters{
              .context = mainScript->context(),
              .fileName = pluginName + "/main.js",
              .script = Filesystem::loadFile(pluginDirectory / pluginMainFile),
              .onModuleLoad =
                  [this](v8::Local<v8::Context> ctx, std::string const& path) {
                      return moduleLoader.load(ctx, path);
                  }})}
        , moduleLoader{}
    {}
    //---------------------------------------------------------------------------------------------------------------------
    void Plugin::Implementation::runModule(v8::Local<v8::Value> defaultExport, Plugin const* plugin)
    {
        auto& ctx = mainScript->context();
        auto object = v8wrap::Object::instantiateClass(ctx, defaultExport);
        if (!object->has("pluginType"))
            throw std::runtime_error("Plugin class needs member called 'pluginType' to identify its type.");
        const auto type = object->get<std::string>("pluginType");
        if (type == "Toolbar")
            pluginClass = std::make_unique<ToolbarPlugin>(std::move(object), plugin);
        else
            throw std::runtime_error("Unknown plugin type called: "s + type);
    }
    //#####################################################################################################################
    Plugin::Plugin(std::string const& pluginName, Api::AllApis const& allApis)
        : impl_{std::make_unique<Implementation>(pluginName, allApis)}
    {
        using namespace v8wrap;
        v8pp::jsmodule minideModule(impl_->mainScript->isolate());
        PluginApi::makeToolbarClass(impl_->mainScript->context(), minideModule);
        PluginApi::makeEditorControlClass(impl_->mainScript->context(), minideModule);
        PluginApi::makeResourceAccessorClass(impl_->mainScript->context(), minideModule, impl_->pluginDataDirectory);

        impl_->moduleLoader.addSynthetic("minide", {{"default", ExportType{minideModule.new_instance()}}});
        exposeGlobals();
    }
    //---------------------------------------------------------------------------------------------------------------------
    void Plugin::run() const
    {
        (void)impl_->mainModule->evaluate();
        extractExports();
    }
    //---------------------------------------------------------------------------------------------------------------------
    void Plugin::extractExports() const
    {
        v8::HandleScope handle_scope(impl_->isolate);
        auto& ctx = impl_->mainScript->context();
        auto ns = impl_->mainModule->getNamespace();
        v8::TryCatch exc(ctx->GetIsolate());
        auto maybeDefaultExport = ns.As<v8::Object>()->Get(ctx, v8pp::to_v8(ctx->GetIsolate(), "default"));
        v8::Local<v8::Value> defaultExport;
        if (!maybeDefaultExport.ToLocal(&defaultExport))
        {
            if (exc.HasCaught())
                v8wrap::rethrowException(ctx->GetIsolate(), exc);
            else
                throw std::runtime_error("There needs to be a default export.");
        }
        LOG() << "Got default export.\n";
        impl_->runModule(defaultExport, this);
    }
    //---------------------------------------------------------------------------------------------------------------------
    void Plugin::initialize(std::weak_ptr<FrontendUserSession> session) const
    {
        impl_->pluginClass->initialize(std::move(session));
    }
    //---------------------------------------------------------------------------------------------------------------------
    std::string Plugin::name() const
    {
        return impl_->name;
    }
    //---------------------------------------------------------------------------------------------------------------------
    void Plugin::exposeGlobals() const
    {
        auto context = impl_->isolate->GetCurrentContext();

        // console
        v8pp::jsmodule console(impl_->isolate);
        console.set("log", &PluginApi::Console::log);
        v8::Local<v8::Object> global = context->Global()->GetPrototype().As<v8::Object>();
        (void)global->Set(context, v8pp::to_v8(impl_->isolate, "console"), console.new_instance());
    }
    //---------------------------------------------------------------------------------------------------------------------
    Plugin::~Plugin() = default;
    //---------------------------------------------------------------------------------------------------------------------
    Plugin::Plugin(Plugin&&) = default;
    //---------------------------------------------------------------------------------------------------------------------
    Plugin& Plugin::operator=(Plugin&&) = default;
    //#####################################################################################################################
}