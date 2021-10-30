#pragma once

#include <backend/server/stream/stream_parser.hpp>
#include <backend/server/stream/dispatcher.hpp>

#include <attender/websocket/server/flexible_session.hpp>

#include <memory>

class BackendControl;

class FrontendUserSession : public attender::websocket::session_base
                          , public std::enable_shared_from_this<FrontendUserSession>  
{
public:
    FrontendUserSession(attender::websocket::connection* owner, std::weak_ptr<BackendControl> server, std::string sessionId);
    ~FrontendUserSession();
    FrontendUserSession(FrontendUserSession&&);
    FrontendUserSession(FrontendUserSession const&) = delete;

    FrontendUserSession& operator=(FrontendUserSession&&);
    FrontendUserSession& operator=(FrontendUserSession const&) = delete;

    void setup();
    void on_close() override;
    void on_text(std::string_view) override;
    void on_binary(char const*, std::size_t) override;
    void on_error(boost::system::error_code, char const*) override {};
    void on_write_complete(std::size_t) override {};

    void writeJson(json const& j);
    void onJson(json const& j);
    void respondWithError(int ref, std::string const& msg);

private:
    struct Implementation;
    std::unique_ptr <Implementation> impl_;
};