#pragma once

#include "../../json.hpp"

namespace Streaming::Messages
{
    struct ServerTime : public JsonSerializable
    {
        std::string toJson() const override;
    };

}
