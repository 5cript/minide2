#pragma once

#include <boost/asio.hpp>

#include <iostream>
#include <atomic>
#include <string_view>
#include <string>

using boost::asio::ip::tcp;

template <typename Handler>
class Server
{
public:
    Server(boost::asio::io_context& io_context, int port)
        : socket_(io_context, tcp::endpoint(tcp::v6(), port))
        , handler_{}
    {
        doReceive();
    }

    ~Server()
    {
        running_.store(false);
    }

    Server& operator=(Server const&) = delete;
    Server(Server const&) = delete;

    void doReceive()
    {
        if (!running_.load())
            return;

        socket_.async_receive(
            boost::asio::buffer(data_, max_length),
            [this](boost::system::error_code ec, std::size_t amount)
            {
                if (!ec && amount > 0)
                {
                    handler_.handleData(std::string_view(data_, amount), [this](std::string_view view){
                        doSend(view);
                    });
                }

                if (ec == boost::system::errc::bad_file_descriptor)
                    return;
                doReceive();
            }
        );
    }

    void doSend(std::string_view view)
    {
        socket_.async_send(
            boost::asio::buffer(view.data(), view.size()),
            [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
            {
            }
        );
    }

private:
    tcp::socket socket_;
    Handler handler_;
    tcp::endpoint sender_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
    std::atomic_bool running_;
};
