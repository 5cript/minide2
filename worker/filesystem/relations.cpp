#include "relations.hpp"

#include <string>

using namespace std::string_literals;
//#####################################################################################################################
/*  root-name: the longest valid sequence of characters that represent a root name are the root name. (C: or //server)
 *  file-name: a block between directory seperators. | dot '.' and dot dot '..' are special file-names.
 *  directory-seperator: unless its the first, a repeated directory seperator is treated the same as a single one:
 *      /a///b is the same as /a/b
 *
 */
//#####################################################################################################################
namespace Filesystem
{
//#####################################################################################################################
    Jail::Jail(sfs::path const& jailRoot)
        : jailRoot_{sfs::canonical(jailRoot)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    bool Jail::isWithinJail(sfs::path const& other) const
    {
        // potentially does more work than necessary.
        return relativeToRoot(other) != std::nullopt;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::optional <sfs::path> Jail::relativeToRoot(sfs::path const& other, bool fakeJailAsRoot) const
    {
        std::error_code ec;
        auto proxi = sfs::proximate(other, jailRoot_, ec);
        if (ec)
            return std::nullopt;
        for (auto const& part : proxi)
        {
            if (part == "..")
                return std::nullopt;
        }
        if (fakeJailAsRoot)
            return {sfs::path{"/"s + jailRoot_.filename().string() + "/" + proxi.generic_string()}};
        else
            return {proxi};
    }
//#####################################################################################################################
}
