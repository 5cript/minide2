#pragma once

#include "../../json.hpp"

namespace Streaming::Messages
{
    struct KeepAlive : public JsonSerializable
    {
        std::string toJson() const override;
    };
}
