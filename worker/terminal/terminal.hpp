#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace Terminal
{
    class Basic
    {
    public:
        Basic
        (
            std::string const& command,
            std::string const& path,
            std::unordered_map <std::string, std::string> const& environment
        );

        void write(std::string const& str);

    private:
        void onStdout(std::string&& str);
        void onStderr(std::string&& str);

    private:
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
