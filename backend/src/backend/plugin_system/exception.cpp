#include <backend/plugin_system/exception.hpp>

#include <fmt/format.h>
#include <v8pp/convert.hpp>

#include <iostream>

namespace PluginSystem
{    
    namespace {
        std::string toString(const v8::String::Utf8Value& value) {
            return *value ? *value : "<string conversion failed>";
        }
    }

    JavaScriptException::JavaScriptException(std::string const& str)
        : std::runtime_error(str.c_str())
    {}

    void rethrowException(v8::Isolate* isolate, v8::TryCatch& jsException)
    {
        v8::HandleScope handle_scope(isolate);
        v8::String::Utf8Value exception(isolate, jsException.Exception());
        const auto exceptionString = toString(exception);
        v8::Local<v8::Message> message = jsException.Message();
        if (message.IsEmpty()) {
            // V8 didn't provide any extra information about this error; just
            // print the exception.
            throw JavaScriptException(exceptionString);
        } else {
            // Print (filename):(line number): (message).
            v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
            v8::Local<v8::Context> context(isolate->GetCurrentContext());
            const auto filenameString = toString(filename);
            int linenum = message->GetLineNumber(context).FromJust();

            // Print line of source code.
            v8::String::Utf8Value sourceline(isolate, message->GetSourceLine(context).ToLocalChecked());
            const auto sourceLineString = toString(sourceline);

            // Print wavy underline (GetUnderline is deprecated).
            int start = message->GetStartColumn(context).FromJust();
            std::string spaceOffset(start, ' ');
            int end = message->GetEndColumn(context).FromJust();
            std::string squiggles(end - start, '~');

            v8::Local<v8::Value> stack_trace_string;
            std::string trace = "";
            if (jsException.StackTrace(context).ToLocal(&stack_trace_string) &&
                stack_trace_string->IsString() &&
                stack_trace_string.As<v8::String>()->Length() > 0) 
            {
                v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
                trace = toString(stack_trace);
            }
            const std::string formatted = fmt::format("{}:{}: {}\n{}\t\n{}{}\n{}", filenameString, linenum, exceptionString, sourceLineString, spaceOffset, squiggles, trace);
            std::cout << formatted << "\n";
            throw JavaScriptException(formatted);
        }
    }
}