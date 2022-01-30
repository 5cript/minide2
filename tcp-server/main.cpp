#include "tcp_server.hpp"

#include <attender/attender/io_context/managed_io_context.hpp>
#include <attender/attender/io_context/thread_pooler.hpp>

#include <iostream>

int main()
{
    attender::managed_io_context <attender::thread_pooler> context;

    using namespace Tcp;
    StreamServer server
    (
        context.get_io_service(),
        [](auto* connection, auto const& ec, auto const& exc)
        {
            std::cout << ec << "\n";
        },
        [](std::shared_ptr <StreamConnection>&& connection)
        {
            connection->setOnClose([](){
                std::cout << "connection died\n";
            });
            connection->readSome([connection](auto begin, auto end)
            {
                std::cout << "read complete\n";
                for (auto i = begin; i != end; ++i)
                {
                    std::cout << *i;
                }
                std::cout << "\n";
                connection->send("hi", [connection](auto, auto) {std::cout << "write complete\n";} /* hold connection until send end */);
                return true;
            },
            [](auto ec)
            {
                std::cout << "read abort\n";
            });
            std::cout << "new stream\n";
        }
    );

    server.start("2580", "::0");

    std::cin.get();

    return 0;
}
