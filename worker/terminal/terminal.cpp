#include "terminal.hpp"

#include <tiny-process-library/process.hpp>
#include <functional>

#include <iostream>

namespace Terminal
{
//#####################################################################################################################
    struct Spawner::Implementation
    {
        Implementation
        (
            std::string const& command,
            std::string const& path,
            std::unordered_map <std::string, std::string> const& environment,
            std::function <void(std::string&&)> readStdout,
            std::function <void(std::string&&)> readStderr
        );

        TinyProcessLib::Process proc;
    };
//---------------------------------------------------------------------------------------------------------------------
    Spawner::Implementation::Implementation
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
            [readStdout](char const* cstr, std::size_t s) {readStdout(std::string{cstr, s});},
            [readStderr](char const* cstr, std::size_t s) {readStderr(std::string{cstr, s});},
            true
        }
    {

    }
//#####################################################################################################################
    Spawner::Spawner
    (
        std::string const& command,
        std::string const& path,
        std::unordered_map <std::string, std::string> const& environment
    )
        : impl_{new Spawner::Implementation
        (
            command,
            path,
            environment,
            [this](std::string&& str){onStdout(std::move(str));},
            [this](std::string&& str){onStderr(std::move(str));}
        )}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void Spawner::onStdout(std::string&& str)
    {
        // TEMPORARY
        std::cout << str;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Spawner::onStderr(std::string&& str)
    {
        // TEMPORARY
        std::cout << str;
    }
//---------------------------------------------------------------------------------------------------------------------
    void Spawner::write(std::string const& str)
    {
        impl_->proc.write(str);
    }
//#####################################################################################################################
}
