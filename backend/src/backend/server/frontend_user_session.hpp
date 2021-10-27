#pragma once

#include <attender/websocket/server/flexible_session.hpp>

#include <memory>

class BackendControl;

class FrontendUserSession : public attender::websocket::session_base
{
public:
    FrontendUserSession(attender::websocket::connection* owner, std::weak_ptr<BackendControl> server, std::string sessionId);
    void on_close() override;
    void on_text(std::string_view data) override {};
    void on_binary(char const* begin, std::size_t amount) override {};
    void on_error(boost::system::error_code ec, char const* where) override {};
    void on_write_complete(std::size_t bytes_transferred) override {};

private:
    std::weak_ptr<BackendControl> server_;
    std::string sessionId_;
};