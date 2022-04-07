#pragma once

#include <variant>

namespace Detail
{
    template <typename... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <typename... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
}

template <typename... VariantTypes, typename... VisitFunctionTypes>
auto visitOverloaded(std::variant<VariantTypes...> const& variant, VisitFunctionTypes&&... visitFunctions)
{
    return std::visit(Detail::overloaded{std::forward<VisitFunctionTypes>(visitFunctions)...}, variant);
}

template <typename... VariantTypes, typename... VisitFunctionTypes>
auto visitOverloaded(std::variant<VariantTypes...>&& variant, VisitFunctionTypes&&... visitFunctions)
{
    return std::visit(Detail::overloaded{std::forward<VisitFunctionTypes>(visitFunctions)...}, std::move(variant));
}
