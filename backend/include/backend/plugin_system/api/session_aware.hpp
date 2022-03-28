#pragma once

#include <backend/server/frontend_user_session.hpp>

#include <memory>

namespace Backend::PluginSystem::PluginApi
{
    template <typename T>
    class SessionAware
    {
      public:
        template <typename OnConstructionCallbackT>
        void contextualize(OnConstructionCallbackT&& onConstructionCallback)
        {
            onConstructionCallback(this);
        }

        void imbueSession(std::weak_ptr<Server::FrontendUserSession> session)
        {
            session_ = std::move(session);
        }

        std::shared_ptr<Server::FrontendUserSession> session()
        {
            return session_.lock();
        }
        std::weak_ptr<Server::FrontendUserSession> weakSession()
        {
            return session_;
        }

      private:
        std::weak_ptr<Server::FrontendUserSession> session_;
    };
}