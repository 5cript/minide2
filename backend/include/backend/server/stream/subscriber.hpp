#pragma once

#include <backend/server/stream/subscription.hpp>
#include <backend/server/stream/dispatcher.hpp>

namespace Backend::Server::Stream
{
    class Subscriber
    {
      public:

      protected:
        virtual ~Subscriber() = default;

        template <typename FunctionT>
        void subscribe(Dispatcher& dispatcher, std::string const& type, FunctionT const& func)
        {
            subscriptions_.push_back(dispatcher.subscribe(type, func));
        }

      private:
        std::vector<std::shared_ptr<Subscription>> subscriptions_;
    };
}