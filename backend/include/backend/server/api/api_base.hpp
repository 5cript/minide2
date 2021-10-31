#pragma once

#include <backend/server/stream/subscription.hpp>
#include <backend/server/stream/dispatcher.hpp>
#include <backend/filesystem/filesystem.hpp>

#include <fstream>

class FrontendUserSession;

namespace Api
{
    class ApiBase
    {
    public:
        void setSession(std::weak_ptr<FrontendUserSession> session)
        {
            session_ = std::move(session);
        }
        
    protected:
        ApiBase(Dispatcher* dispatcher)
            : dispatcher_{dispatcher}
            , session_{}
        {}
        virtual ~ApiBase() = default;

        template <typename FunctionT>
        void subscribe(std::string const& type, FunctionT const& func)
        {
            subscriptions_.push_back(dispatcher_->subscribe(type, func));
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
        Dispatcher* dispatcher_;
        std::weak_ptr<FrontendUserSession> session_;
        std::vector<std::shared_ptr<Subscription>> subscriptions_;
    };
}

#define DECLARE_API(Name) \
    Name(Dispatcher* dispatcher) \
        : ApiBase(dispatcher) \
    { \
        doSubscribe(); \
    }