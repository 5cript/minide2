#pragma once

#include <backend/server/stream/subscription.hpp>

#include <backend/json.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <optional>

class Dispatcher final
{
  public:
    [[nodiscard]] std::shared_ptr<Subscription>
    subscribe(std::string const& type, std::function<bool(Subscription::ParameterType const&)> const& callback);

    [[nodiscard]] std::shared_ptr<Subscription>
    subscribe(std::string const& type, std::function<void(Subscription::ParameterType const&)> const& callback);

    void dispatch(json const& msg);

  private:
    std::unordered_multimap<std::string, std::weak_ptr<Subscription>> m_subscribers;
    std::mutex m_subscriberGuard;
};