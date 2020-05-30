#pragma once

#include "../../json.hpp"

namespace Streaming::Messages
{
    struct Welcome : public JsonSerializable
    {
        int id;

        std::string toJson() const override;
    };
}
