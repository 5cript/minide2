#pragma once

#include <SimpleJSON/parse/jsd.hpp>
#include <SimpleJSON/stringify/jss.hpp>
#include <SimpleJSON/stringify/jss_fusion_adapted_struct.hpp>

#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/*
template <typename T>
struct EnableJson
    : public JSON::Serializable <T>
    , public JSON::Parsable <T>
{
    std::string dump() const
    {
        std::stringstream sstr;
        JSON::stringify(sstr, "", *static_cast <T*>(this));
        return sstr.str();
    }

    void parse(std::string const& data)
    {
        *static_cast <T>(this) = JSON::make_from_json <T> (data);
    }
};
*/

#define ADAPT(...) BOOST_JSON_ADAPT_STRUCT(__VA_ARGS__)
