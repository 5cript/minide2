#pragma once

#include <filesystem>

namespace sfs = std::filesystem;

namespace Filesystem
{
    using path = std::filesystem::path;

    /**
     *  Normalizes the path, but uses forward slash.
     */
    path linux_normalize(path const& p);
}
