#include <backend/server/stream/dispatcher.hpp>

#include <backend/log.hpp>

//#####################################################################################################################
void Dispatcher::dispatch(json const& msg)
{
    const std::string type = msg["type"];

    std::scoped_lock lock{m_subscriberGuard};
    auto [begin, end] = m_subscribers.equal_range(type);
    if (begin == end)
        LOG() << "No subscriber for: " << type << "\n";

    while (begin != end)
    {
        if (auto held = begin->second.lock(); held)
        {
            if (!std::invoke(*held, msg))
                break;
            ++begin;
        }
        else
            begin = m_subscribers.erase(begin);
    }
}
//---------------------------------------------------------------------------------------------------------------------
std::shared_ptr<Subscription>
Dispatcher::subscribe(std::string const& messageType, std::function<bool(Subscription::ParameterType const&)> const& callback)
{
    std::scoped_lock lock{m_subscriberGuard};
    std::shared_ptr<Subscription> subscriber = std::make_shared<Subscription>(callback);
    m_subscribers.emplace(messageType, subscriber);
    return subscriber;
}
//---------------------------------------------------------------------------------------------------------------------
std::shared_ptr<Subscription>
Dispatcher::subscribe(std::string const& type, std::function<void(Subscription::ParameterType const&)> const& callback)
{
    return subscribe(type, std::function<bool(Subscription::ParameterType const&)>{
        [callback](Subscription::ParameterType const& p) -> bool{
            callback(p);
            return true;
        }
    });
}
//#####################################################################################################################