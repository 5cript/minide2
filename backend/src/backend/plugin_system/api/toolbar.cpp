#include <backend/plugin_system/api/toolbar.hpp>
#include <backend/utility/uuid.hpp>
#include <backend/server/frontend_user_session.hpp>
#include <backend/plugin_system/plugin.hpp>
#include <backend/plugin_system/api/console.hpp>
#include <backend/json.hpp>

#include <v8wrap/array.hpp>
#include <v8wrap/json.hpp>

#include <fmt/core.h>

#include <iostream>
#include <functional>

namespace Backend::PluginSystem::PluginApi
{
    namespace
    {
        template <template <typename...> typename HandleType>
        std::string setIdOnObject(v8wrap::Object<HandleType>& obj)
        {
            std::string id;
            if (obj.has("id"))
            {
                id = obj.template get<std::string>("id") + "_" + makeUuid();
                obj.template set("userId", obj.template get<std::string>("id"));
            }
            else
                id = makeUuid();
            obj.template set("id", id);
            return id;
        }
    }
    //#####################################################################################################################
    struct Toolbar::Implementation
    {
        std::string id = makeUuid();
    };
    //#####################################################################################################################
    const std::string Toolbar::pluginType = "Toolbar";
    //---------------------------------------------------------------------------------------------------------------------
    Toolbar::Toolbar()
        : impl_{std::make_unique<Implementation>()}
    {}
    //---------------------------------------------------------------------------------------------------------------------
    Toolbar::~Toolbar() = default;
    //---------------------------------------------------------------------------------------------------------------------
    void Toolbar::registerActionHandler(ToolbarAction options)
    {
        if (options.decorated->has("action"))
        {
            if (auto sess = options.toolbar->session(); sess)
            {
                options.toolbar->subscribe(
                    sess->getSubscriptionDispatcher(),
                    options.path,
                    [toolbar = options.toolbar, callback = options.callback](json const& j) {
                        toolbar->owner_->performAsyncScriptAction([&callback, toolbar, &j](auto context) {
                            auto plugin = v8::Local<v8::Value>::New(context->GetIsolate(), toolbar->pluginClass_);
                            callback(context, v8wrap::Object<>{context, plugin}, j);
                        });
                    });
            }
        }
    }
    //---------------------------------------------------------------------------------------------------------------------
    void Toolbar::initialize(v8::Local<v8::Context> context)
    {
        if (auto sess = session(); sess)
        {
            v8::HandleScope scope{context->GetIsolate()};
            auto plugin = v8wrap::Object{context, v8::Local<v8::Value>::New(context->GetIsolate(), pluginClass_)};
            v8wrap::Array elements{context, v8wrap::Object{context, plugin.asValue()}.call("initialize")};
            v8wrap::Object toolbarDefinition{context};

            toolbarDefinition.set("name", owner_->name());
            toolbarDefinition.set("id", impl_->id);
            toolbarDefinition.set("event", "toolbarInitialized");
            toolbarDefinition.set("items", elements.asValue());
            plugin.set("id", impl_->id);
            plugin.set("items", elements.asValue());

            sess->enqueue_write([result{v8wrap::JSON::stringify(context, toolbarDefinition.asValue())}](
                                    attender::websocket::session_base* sess, std::size_t) {
                static_cast<Backend::Server::FrontendUserSession*>(sess)->writeText(result);
            });
        }
    }
    //---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Value> Toolbar::makeMenu(v8::FunctionCallbackInfo<v8::Value> const& args)
    {
        v8::EscapableHandleScope scope{args.GetIsolate()};
        auto ctx = args.GetIsolate()->GetCurrentContext();
        if (args.Length() != 1)
            throw std::runtime_error("Expecting exactly one argument to makeMenu, being an array.");
        v8wrap::Array input{ctx, args[0]};
        v8wrap::Array<v8::Local> transformed{ctx, v8::Array::New(args.GetIsolate(), input.size())};

        // Transform menu items:
        std::transform(std::begin(input), std::end(input), std::begin(transformed), [&](v8::Local<v8::Value> val) {
            v8::EscapableHandleScope scope{args.GetIsolate()};
            v8wrap::Object entry{ctx, val};
            if (!entry.has("label"))
                entry.set("label", "MISSING_LABEL");
            if (!entry.has("type"))
            {
                if (entry.has("pngbase64"))
                    entry.set("type", "IconButton");
                else
                    entry.set("type", "Button");
            }
            setIdOnObject(entry);
            return scope.Escape(static_cast<v8::Local<v8::Value>>(entry));
        });

        v8wrap::Object decorated{ctx, v8::Object::New(args.GetIsolate())};
        decorated.set("type", "Menu");
        decorated.set("entries", static_cast<v8::Local<v8::Value>>(transformed));
        setIdOnObject(decorated);
        decorated.set("generated", true);
        return scope.Escape(static_cast<v8::Local<v8::Value>>(decorated));
    }
    //---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Value> Toolbar::makeSplitter(v8::FunctionCallbackInfo<v8::Value> const& args)
    {
        v8::EscapableHandleScope scope{args.GetIsolate()};
        auto ctx = args.GetIsolate()->GetCurrentContext();
        v8wrap::Object decorated{ctx, v8::Object::New(args.GetIsolate())};
        decorated.set("type", "Splitter");
        setIdOnObject(decorated);
        decorated.set("generated", true);
        return scope.Escape(static_cast<v8::Local<v8::Value>>(decorated));
    }
    //---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Value> Toolbar::makeIconButton(v8::FunctionCallbackInfo<v8::Value> const& args)
    {
        v8::EscapableHandleScope scope{args.GetIsolate()};
        if (args.Length() != 1)
            throw std::runtime_error("Expecting exactly one argument to makeMenu, being an object.");
        auto ctx = args.GetIsolate()->GetCurrentContext();
        v8wrap::Object decorated{ctx, args[0]};
        const auto buttonId = setIdOnObject(decorated);

        auto action = [](auto context, auto& plugin, json const& j) {
            // std::cout << v8wrap::JSON::stringify(context, plugin.asValue()) << "\n";
            // TODO:
        };

        registerActionHandler({
            .toolbar = this,
            .decorated = &decorated,
            .path = fmt::format("/api/toolbar/{}/callAction/{}", impl_->id, buttonId),
            .callback = action,
        });

        if (decorated.has("pngbase64"))
            decorated.set("type", "IconButton");
        else
            decorated.set("type", "Button");
        decorated.set("generated", true);
        return scope.Escape(static_cast<v8::Local<v8::Value>>(decorated));
    }
    //#####################################################################################################################
}