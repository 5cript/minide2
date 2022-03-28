#include <backend/server/stream/subscription.hpp>

namespace Backend::Server::Stream
{
    //#####################################################################################################################
    Subscription::Subscription(FunctionType cb)
        : m_callback{std::move(cb)}
    {}
    //#####################################################################################################################
}