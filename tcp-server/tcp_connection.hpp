#pragma once

// taking connection from attender
#include <attender/http/http_connection.hpp>
#include <attender/http/http_secure_connection.hpp>

namespace Tcp
{
    using ConnectionInterface = attender::http_connection_interface;
    using SecureConnection = attender::http_secure_connection;
    using Connection = attender::http_connection;
}
