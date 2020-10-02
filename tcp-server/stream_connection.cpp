#include "stream_connection.hpp"

#include <attender/attender/tcp_connection_interface.hpp>
#include <attender/attender/tcp_server_interface.hpp>
#include <attender/attender/connection_manager.hpp>

namespace Tcp
{
//#####################################################################################################################
    StreamConnection::StreamConnection(attender::tcp_connection_interface* connection)
        : connection_{connection}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    StreamConnection::~StreamConnection()
    {
        close();
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamConnection::close()
    {
        onClose_();
        connection_->get_parent()->get_connections()->remove(connection_);
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamConnection::readSome(
        std::function <bool(
            attender::tcp_connection_interface::buffer_iterator begin,
            attender::tcp_connection_interface::buffer_iterator end
        )> const& onRecv,
        std::function <void(boost::system::error_code)> const& onError
    )
    {
        connection_->set_read_callback([this, onError, onRecv](auto ec)
        {
            if (ec)
            {
                onError(ec);
                // if the read callback contains a shared_ptr, to clear it.
                connection_->set_read_callback([](auto){});
            }
            else if (onRecv(connection_->begin(), connection_->end()))
            {
                connection_->set_read_callback([](auto){});
            }
        });
        connection_->read();
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamConnection::setOnClose(std::function <void()> onClose)
    {
        onClose_ = onClose;
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamConnection::send(std::string const& data, std::function <void(boost::system::error_code, std::size_t)> onWriteComplete)
    {
        connection_->write(data, onWriteComplete);
    }
//#####################################################################################################################
}
