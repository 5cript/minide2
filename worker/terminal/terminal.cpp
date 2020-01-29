#include "terminal.hpp"

#include <tiny-process-library/process.hpp>
#include <functional>

namespace Terminal
{
//#####################################################################################################################
    struct Basic::Implementation
    {
        Implementation(std::string const& process);

        TinyProcessLib::Process proc;
    }
//---------------------------------------------------------------------------------------------------------------------
    Basic::Implementation::Implementation
    (
        std::string const& command,
        std::string const& path,
        std::unordered_map <std::string, std::string> const& environment,
        std::function <void(std::string&&)> readStdout,
        std::function <void(std::string&&)> readStderr
    )
        : proc
        {
            command,
            path,
            environment,
            readStdout,
            readStdin,
            true
        }
    {

    }
//#####################################################################################################################
    Basic::Basic
    (
        std::string const& command,
        std::string const& path,
        std::unordered_map <std::string, std::string> const& environment
    )
        : impl_{new Basic::Implementation
        (
            command,
            path,
            environment,
            [](std::string&& str){onStdout(std::move(str));},
            [](std::string&& str){onStderr(std::move(str));}
        )}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void Basic::onStdout(std::string&& str)
    {
        // TEMPORARY
        std::cout << str;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Basic::onStderr(std::string&& str)
    {
        // TEMPORARY
        std::cout << str;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Basic::write(std::string const& str)
    {
        impl_->proc.write(str);
    }
//#####################################################################################################################
}
