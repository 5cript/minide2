#pragma once

#include <backend/json.hpp>

#include <functional>

class Subscription
{
public:
    using ParameterType = json;
    using FunctionType = std::function<bool(ParameterType const&)>;

    explicit Subscription(FunctionType cb);
    bool operator()(ParameterType const& param)
    {
        return m_callback(param);
    }

private:
    FunctionType m_callback;
};