#include <backend/settings/environment.hpp>

#include <set>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/range/as_literal.hpp>
#include <iostream>

// Case-insensitive generic string comparator.
namespace
{
    struct range_iless
    {
        template< typename InputRange1, typename InputRange2 >
        bool operator()( InputRange1 const& r1, InputRange2 const& r2 ) const
        {
            using std::begin; using std::end;

            auto ir1 = boost::as_literal( r1 );
            auto ir2 = boost::as_literal( r2 );

            return std::lexicographical_compare(
                begin( ir1 ), end( ir1 ),
                begin( ir2 ), end( ir2 ),
                boost::is_iless{}
            );
        }
    };

    template <class Key, class Allocator = std::allocator<Key>>
    using iset = std::set <Key, range_iless, Allocator>;
}

namespace SettingParts
{
//#####################################################################################################################
    std::string Environment::mergePath(char pathSplit) const
    {
        std::string pathStr{};
        for (auto const& elem : path)
            pathStr += elem + pathSplit;
        pathStr.pop_back();
        return pathStr;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::unordered_map <std::string, std::string> Environment::compile(char pathSplit) const
    {
        std::unordered_map <std::string, std::string> env;
        env["PATH"] = mergePath(pathSplit);
        env.insert(std::begin(variables), std::end(variables));
        return env;
    }
//---------------------------------------------------------------------------------------------------------------------
    Environment Environment::merge(Environment const& other, bool pathsAreCaseInsensitive)
    {
        Environment merged;
        merged.variables = variables;
        for (auto const& [key, value] : other.variables)
            merged.variables[key] = value;

        auto mergePath = [](auto set, std::vector <std::string> const& lhs, std::vector <std::string> const& rhs)
        {
            for (auto const& l : lhs)
            {
                set.insert(l);
            }
            for (auto const& r : rhs)
            {
                set.insert(r);
            }
            return std::vector <std::string>{std::begin(set), std::end(set)};
        };

        if (pathsAreCaseInsensitive)
            merged.path = mergePath(iset <std::string>{}, path, other.path);
        else
            merged.path = mergePath(std::set <std::string>{}, path, other.path);
        return merged;
    }
//#####################################################################################################################
    void to_json(json& j, Environment const& env)
    {
        j = json
        {
            {"path", env.path},
            {"variables", env.variables},
            {"inherits", env.inherits}
        };
    }
//---------------------------------------------------------------------------------------------------------------------
    void from_json(json const& j, Environment& env)
    {
        j.at("path").get_to(env.path);
        j.at("variables").get_to(env.variables);
        j.at("inherits").get_to(env.inherits);
    }
//#####################################################################################################################
}
