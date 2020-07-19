#pragma once

#include "filesystem.hpp"

namespace Filesystem
{
    /**
     *  Not meant to be 100% abuse proof.
     */
    class Jail
    {
    public:
        Jail(sfs::path const& jailRoot);

        /**
         *  Is the given path within the jail.
         */
        bool isWithinJail(sfs::path const& other) const;

        /**
         *  Will return a path, that is relative to the jail, if the path is within the jail.
         */
        std::optional <sfs::path> relativeToRoot(sfs::path const& other, bool fakeJailAsRoot = false) const;

        sfs::path fakeJailAsRoot(sfs::path const& other) const;

    private:
        sfs::path jailRoot_;
    };
}
