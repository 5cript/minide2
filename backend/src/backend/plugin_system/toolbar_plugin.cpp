#include <backend/plugin_system/toolbar_plugin.hpp>
#include <backend/plugin_system/api/console.hpp>
#include <backend/server/frontend_user_session.hpp>

#include <v8wrap/array.hpp>
#include <v8wrap/json.hpp>

namespace PluginSystem
{
    //#####################################################################################################################
    ToolbarPlugin::ToolbarPlugin(std::unique_ptr<v8wrap::Object>&& pluginClass)
        : PluginImplementation{std::move(pluginClass)}
    {}
    //---------------------------------------------------------------------------------------------------------------------
    void ToolbarPlugin::initialize(std::weak_ptr<FrontendUserSession>&& session)
    {
        PluginImplementation::initialize(std::move(session));

        if (auto sess = session_.lock(); sess)
        {
            v8::HandleScope scope{pluginClass_->isolate()};
            v8wrap::Array elements{pluginClass_->context(), pluginClass_->call("initialize")};
            v8wrap::Object toolbarDefinition{pluginClass_->context()};
            toolbarDefinition.set("pluginType", "toolbar");
            toolbarDefinition.set("elements", elements.asValue());

            sess->enqueue_write([result{v8wrap::JSON::stringify(pluginClass_->context(), toolbarDefinition.asValue())}](
                                    attender::websocket::session_base* sess, std::size_t) {
                static_cast<FrontendUserSession*>(sess)->writeText(result);
            });
        }
    }
    //#####################################################################################################################
}