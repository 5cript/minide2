#pragma once

// taking connection from attender
#include <attender/attender/tcp_connection.hpp>
#include <attender/attender/tcp_secure_connection.hpp>

namespace Tcp
{
    using ConnectionInterface = attender::tcp_connection_interface;
    using SecureConnection = attender::tcp_secure_connection;
    using Connection = attender::tcp_connection;
}
