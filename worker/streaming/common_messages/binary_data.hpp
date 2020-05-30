#pragma once

#include "../../json.hpp"

namespace Streaming::Messages
{
    struct BinaryData : public JsonSerializable
    {
        std::size_t dataSize;
        std::string toJson() const override;

        BinaryData(std::size_t dataSize)
            : dataSize{dataSize}
        {
        }
    };
}
