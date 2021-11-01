#include <backend/plugin_system/isolate.hpp>

namespace PluginSystem
{
//#####################################################################################################################
struct Isolate::Implementation
{
    v8::Isolate::CreateParams createParams;
    std::unique_ptr<v8::Isolate, void(*)(v8::Isolate*)> isolate;
    v8::HandleScope handleScope;

    Implementation();
    ~Implementation();
};
//---------------------------------------------------------------------------------------------------------------------
Isolate::Implementation::Implementation()
    : createParams{
        []()
        {
            v8::Isolate::CreateParams params;
            params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
            return params;
        }()
    }
    , isolate{
        v8::Isolate::New(createParams), 
        [](v8::Isolate* iso) 
        {
            iso->Dispose();
        }
    }
    , handleScope{isolate.get()}
{
}
//---------------------------------------------------------------------------------------------------------------------
Isolate::Implementation::~Implementation()
{
    delete createParams.array_buffer_allocator;
}
//#####################################################################################################################
Isolate::Isolate()
    : impl_{std::make_unique<Implementation>()}
{
}
//---------------------------------------------------------------------------------------------------------------------
v8::Isolate* Isolate::handle()
{
    return impl_->isolate.get();
}
//---------------------------------------------------------------------------------------------------------------------
Isolate::operator v8::Isolate*()
{
    return handle();
}
//---------------------------------------------------------------------------------------------------------------------
Isolate::~Isolate() = default;
//---------------------------------------------------------------------------------------------------------------------
Isolate::Isolate(Isolate&&) = default;
//---------------------------------------------------------------------------------------------------------------------
Isolate& Isolate::operator=(Isolate&&) = default;
//#####################################################################################################################
}