#pragma once

#include "tcp_connection.hpp"

namespace Tcp
{
    class StreamConnection
    {
    public:
        StreamConnection(attender::http_connection_interface* connection);
        ~StreamConnection();

        StreamConnection& operator=(StreamConnection const&) = delete;
        StreamConnection& operator=(StreamConnection&&) = default;
        StreamConnection(StreamConnection const&) = delete;
        StreamConnection(StreamConnection&&) = default;

        void readSome(
            std::function <bool( // <-- return true if you want the read handler to be removed
                attender::http_connection_interface::buffer_iterator begin,
                attender::http_connection_interface::buffer_iterator end
            )> const& onRecv,
            std::function <void(boost::system::error_code)> const& onError = [](auto){}
        );

        void send(std::string const& data, std::function <void(boost::system::error_code, std::size_t)> onWriteComplete = [](auto, auto){});

        void setOnClose(std::function <void()> onClose);

    private:
        // private now, because afair multiple close calls are not ok. Connection lifetime is object lifetime
        void close();

    private:
        attender::http_connection_interface* connection_;
        std::function <void()> onClose_;
    };
}
