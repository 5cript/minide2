#include <backend/plugin_system/toolbar_plugin.hpp>
#include <backend/plugin_system/plugin.hpp>
#include <backend/plugin_system/api/console.hpp>

#include <backend/server/frontend_user_session.hpp>
#include <backend/utility/uuid.hpp>

#include <v8wrap/array.hpp>
#include <v8wrap/json.hpp>

namespace Backend::PluginSystem
{
    //#####################################################################################################################
    void ToolbarPlugin::initialize(std::weak_ptr<Server::FrontendUserSession>&& session)
    {
        PluginImplementation::initialize(std::move(session));

        if (auto sess = session_.lock(); sess)
        {
            auto subscribe = [this, &sess](auto const& type, auto const& func) {
                this->subscribe(sess->getSubscriptionDispatcher(), type, func);
            };

            v8::HandleScope scope{pluginClass_->isolate()};
            v8wrap::Array elements{pluginClass_->context(), pluginClass_->call("initialize")};
            v8wrap::Object toolbarDefinition{pluginClass_->context()};

            toolbarDefinition.set("name", owner_->name());
            toolbarDefinition.set("id", makeUuid());
            toolbarDefinition.set("event", "toolbarInitialized");
            toolbarDefinition.set("items", elements.asValue());

            sess->enqueue_write([result{v8wrap::JSON::stringify(pluginClass_->context(), toolbarDefinition.asValue())}](
                                    attender::websocket::session_base* sess, std::size_t) {
                static_cast<Backend::Server::FrontendUserSession*>(sess)->writeText(result);
            });
        }
    }
    //#####################################################################################################################
}