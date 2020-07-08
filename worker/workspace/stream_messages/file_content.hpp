#pragma once

#include "../../json.hpp"

#include <optional>
#include <string>
#include <utility>

namespace Streaming::Messages
{
    /**
     *  Used for sending the content of a directory (recursive).
     */
    class FileContent : public JsonSerializable
    {
    public:
        std::vector <std::string> chunks;
        std::string path;
        std::string flag;
        std::optional <int> line;
        std::optional <int> linePos;

        std::string toJson() const override;

        /**
         *  If there was an error the return value contains the error message.
         */
        std::string load(std::string const& file, bool forceLoad, long long max, long long forceMax, long long chunkSize);
    };
}
