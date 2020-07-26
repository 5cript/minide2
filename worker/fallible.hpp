#pragma once

#include <optional>
#include <type_traits>

struct VoidResult{};

template <typename ErrorType, typename SuccessType>
class Fallible
{
public:
    using error_type = ErrorType;
    using success_type = std::conditional_t <
        std::is_same_v <SuccessType, void>,
        VoidResult,
        SuccessType
    >
    ;

    Fallible(error_type e)
        : error_{std::move(e)}
    {
    }

    Fallible(success_type st)
        : result_{std::move(st)}
    {
    }

    explicit operator bool() const
    {
        return result_.operator bool();
    }

    bool didFail() const
    {
        return error_.operator bool();
    }

    std::optional <error_type> error()
    {
        return error_;
    }
    std::optional <success_type> result()
    {
        return result_;
    }

    error_type error_value()
    {
        if (error_)
            return error_.value();
        else
            throw std::runtime_error("there is no error");
    }

private:
    std::optional <error_type> error_;
    std::optional <success_type> result_;
};

template <
    typename ErrorType,
    typename SuccessType,
    typename _ = std::enable_if_t <std::is_same_v<SuccessType, void>>
>
Fallible <ErrorType, SuccessType> fallibleSuccess()
{
    using ftype = Fallible <ErrorType, SuccessType>;
    return ftype{VoidResult{}};
}

template <
    typename ErrorType,
    typename SuccessType
>
Fallible <ErrorType, SuccessType> fallibleSuccess(SuccessType success)
{
    return Fallible <ErrorType, SuccessType>{std::move(success)};
}
