#include <attender/websocket/server/server.hpp>
#include <attender/websocket/server/connection.hpp>
#include <attender/session/uuid_session_cookie_generator.hpp>

#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace Backend::Server
{
    class BackendControl : public std::enable_shared_from_this<BackendControl>
    {
      public:
        BackendControl(boost::asio::io_context* service);

        void start(std::string const& port);

        bool removeSession(std::string const& id);

      private:
        attender::websocket::server server_;
        std::mutex guard_;
        std::unordered_map<std::string, std::shared_ptr<attender::websocket::connection>> connections_;
        attender::uuid_generator generator_;
    };
}