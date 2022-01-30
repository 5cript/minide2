#include <backend/plugin_system/api/toolbar.hpp>

namespace PluginSystem::PluginApi
{
    //#####################################################################################################################
    v8::Local<v8::Value> Toolbar::makeMenu(v8::FunctionCallbackInfo<v8::Value> const& args)
    {
        v8::EscapableHandleScope scope{args.GetIsolate()};
        auto ctx = args.GetIsolate()->GetCurrentContext();
        if (args.Length() != 1)
            throw std::runtime_error("Expecting exactly one argument to makeMenu, being an array.");
        v8wrap::Array input{ctx, args[0]};
        v8wrap::Array transformed{ctx, v8::Array::New(args.GetIsolate(), input.size())};
        std::transform(std::begin(input), std::end(input), std::begin(transformed), [&](v8::Local<v8::Value> val) {
            v8::EscapableHandleScope scope{args.GetIsolate()};
            v8wrap::Object entry{ctx, val};
            if (!entry.has("label"))
                entry.set("label", "MISSING_LABEL");
            if (!entry.has("type"))
            {
                if (entry.has("pngbase64"))
                    entry.set("type", "icon_button");
                else
                    entry.set("type", "button");
            }
            return scope.Escape(static_cast<v8::Local<v8::Value>>(entry));
        });
        v8wrap::Object decorated{ctx, v8::Object::New(args.GetIsolate())};
        decorated.set("type", "menu");
        decorated.set("entries", static_cast<v8::Local<v8::Value>>(transformed));
        return decorated;
    }
    //---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Value> Toolbar::makeSplitter(v8::FunctionCallbackInfo<v8::Value> const& args)
    {
        auto ctx = args.GetIsolate()->GetCurrentContext();
        v8wrap::Object decorated{ctx, v8::Object::New(args.GetIsolate())};
        decorated.set("type", "splitter");
        return decorated;
    }
    //---------------------------------------------------------------------------------------------------------------------
    v8::Local<v8::Value> Toolbar::makeIconButton(v8::FunctionCallbackInfo<v8::Value> const& args)
    {
        if (args.Length() != 1)
            throw std::runtime_error("Expecting exactly one argument to makeMenu, being an object.");
        auto ctx = args.GetIsolate()->GetCurrentContext();
        v8wrap::Object decorated{ctx, args[0]};

        if (decorated.has("pngbase64"))
            decorated.set("type", "icon_button");
        else
            decorated.set("type", "button");
        return decorated;
    }
    //#####################################################################################################################
}