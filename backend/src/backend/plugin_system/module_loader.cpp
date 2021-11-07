#include <backend/plugin_system/module_loader.hpp>

#include <backend/log.hpp>
#include <v8pp/convert.hpp>

namespace PluginSystem
{
//#####################################################################################################################
    v8::MaybeLocal<v8::Module> ModuleLoader::load(v8::Local<v8::Context> context, std::string const& path)
    {
        LOG() << "Loading module: " << path << "\n";
        if (path == "minide/plugin")
        {
            return loadSynthetic(context, path, v8pp::to_v8(context->GetIsolate(), "default"));
        }
        return v8::MaybeLocal<v8::Module>{};
    }
//---------------------------------------------------------------------------------------------------------------------
    v8::MaybeLocal<v8::Module> ModuleLoader::loadSynthetic(v8::Local<v8::Context> context, std::string const& path, v8::Local<v8::Value> value)
    {
        LOG() << "Loading synthetic module: " << path << "\n";
        auto mod = *loadedModules_.insert(std::end(loadedModules_), std::make_shared <Module>(
            Module::SyntheticCreationParameters{
                .context = context,
                .fileName = path,
                .exported = value,
                .onModuleLoad = [this](v8::Local<v8::Context> c, std::string const& p){
                    return load(c, p);
                }
            }
        ));
        mod->setup();
        return mod->getModule();
    }
//---------------------------------------------------------------------------------------------------------------------
}