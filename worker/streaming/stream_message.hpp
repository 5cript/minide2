#pragma once

#include "../json.hpp"

#include <memory>
#include <variant>
#include <vector>
#include <string>
#include <utility>
#include <type_traits>

namespace Streaming
{
    using BinaryData = std::variant <
        std::monostate,
        std::vector <char>,
        std::string
    >;

    struct Message
    {
        std::unique_ptr <JsonSerializable> head;
        BinaryData data;

        Message(std::unique_ptr <JsonSerializable>&& up)
            : head{std::move(up)}
            , data{}
        {
        }

        Message(std::unique_ptr <JsonSerializable>&& up, BinaryData&& data)
            : head{std::move(up)}
            , data{std::move(data)}
        {
        }

        Message(JsonSerializable* seri)
            : head(seri)
            , data{}
        {
        }

        Message(JsonSerializable* seri, BinaryData&& data)
            : head(seri)
            , data{std::move(data)}
        {
        }
    };

    template <typename T, typename... Args>
    Message makeMessage(Args&&... args)
    {
        return Message(new T(std::forward <Args&&>(args)...));
    }
}
