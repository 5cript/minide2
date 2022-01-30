#pragma once

#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct JsonSerializable
{
    virtual std::string toJson() const = 0;
    virtual ~JsonSerializable() = default;
};
