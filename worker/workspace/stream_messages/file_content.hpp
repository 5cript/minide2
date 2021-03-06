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
        std::vector <std::string> chunks = {};
        std::string path = "";
        std::string flag = "";
        std::optional <int> line = std::nullopt;
        std::optional <int> linePos = std::nullopt;
        std::optional <std::string> message = std::nullopt;
        bool isAbsolutePath = false;
        bool dontReloadIfAlreadyOpen = false;

        std::string toJson() const override;

        /**
         *  If there was an error the return value contains the error message.
         */
        std::string load(std::string const& file, bool forceLoad, long long max, long long forceMax, long long chunkSize);
    };
}
