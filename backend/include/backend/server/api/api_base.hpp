#pragma once

#include <backend/server/stream/subscription.hpp>
#include <backend/server/stream/subscriber.hpp>
#include <backend/server/stream/dispatcher.hpp>
#include <backend/filesystem/filesystem.hpp>

#include <fstream>

namespace Backend::Server
{
    class FrontendUserSession;
}

namespace Backend::Server::Api
{
    class ApiBase : public Stream::Subscriber
    {
      public:
        void setSession(std::weak_ptr<FrontendUserSession> session)
        {
            session_ = std::move(session);
        }

      protected:
        ApiBase(Stream::Dispatcher* dispatcher)
            : dispatcher_{dispatcher}
            , session_{}
        {}
        virtual ~ApiBase() = default;

        template <typename FunctionT>
        void subscribe(std::string const& type, FunctionT const& func)
        {
            Stream::Subscriber::subscribe(*dispatcher_, type, func);
        }

        virtual void doSubscribe() = 0;

        std::shared_ptr<FrontendUserSession> session()
        {
            return session_.lock();
        }

        std::weak_ptr<FrontendUserSession> weakSession()
        {
            return session_;
        }

      private:
        Stream::Dispatcher* dispatcher_;
        std::weak_ptr<FrontendUserSession> session_;
        std::vector<std::shared_ptr<Stream::Subscription>> subscriptions_;
    };
}

#define DECLARE_API(Name) \
    Name(::Backend::Server::Stream::Dispatcher* dispatcher) \
        : ::Backend::Server::Api::ApiBase(dispatcher) \
    { \
        doSubscribe(); \
    }