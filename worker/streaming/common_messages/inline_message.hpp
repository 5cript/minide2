#pragma once

#include <string>
#include "../../json.hpp"

namespace Streaming::Messages
{
    struct InlineMessage : public JsonSerializable
    {
        std::string type;
        mutable json payload;

        InlineMessage() = default;
        InlineMessage(std::string type, json const& payload = {})
            : type{std::move(type)}
            , payload(payload)
        {
        }
        InlineMessage(InlineMessage&&) = default;

        std::string toJson() const override;
    };
}
