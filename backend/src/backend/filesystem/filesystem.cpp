#include <backend/filesystem/filesystem.hpp>

#include <string>

namespace Filesystem
{
    //#####################################################################################################################
    namespace
    {
        [[maybe_unused]] void replaceAll(std::string& in, std::string const& needle, std::string const& replacement)
        {
            if (needle.empty())
                return;

            for (std::size_t start = in.find(needle, 0); start != std::string::npos; start = in.find(needle, start))
            {
                in.replace(start, needle.length(), replacement);
                start += replacement.length();
            }
        }
    }
    //#####################################################################################################################
    path linux_normalize(path const& p)
    {
        /*
        if (p.empty())
            return p;

        auto generic = p.generic_string();
        replaceAll(generic, "\\", "/");
        replaceAll(generic, "./", "");

        path rootName = p.root_name();
        path rootDirectory = p.root_directory();

        path dotDotRemover(generic);
        auto partIterator = std::begin(dotDotRemover);

        // ignore root name for now
        if (!rootName.empty())
            partIterator++;
        if (partIterator == std::end(dotDotRemover))
            return path{generic};

        // ignore root directory in main loop,
        // also remove all dot-dots going up after root
        if (!rootDirectory.empty())
        {
            partIterator++;
            for (;  partIterator != std::end(dotDotRemover) && *partIterator == ".."; ++partIterator)
            { }
        }
        if (partIterator == std::end(dotDotRemover))
            return path{generic};

        std::vector <path> parts;
        for (; partIterator != std::end(dotDotRemover); ++partIterator)
        {
            if (partIterator->empty())
                continue;

            auto lookaheadOne = partIterator;
            lookaheadOne++;

            if
            (
                lookaheadOne != std::end(dotDotRemover) &&
                *lookaheadOne == ".." &&
                *partIterator != "." &&
                *partIterator != ".."
            )
            {
                ++partIterator; // skip
                continue;
            }
            parts.push_back(*partIterator);
        }
        if (!parts.empty() && *parts.rbegin() == "..")
            parts.pop_back();

        path result;
        if (!parts.empty())
            result = path{rootName.string() + rootDirectory.string() + parts[0].string()};
        else
            result = path{rootName.string() + rootDirectory.string()};
        if (parts.size() > 1)
        {
            for (auto iter = parts.begin() + 1; iter != parts.end(); ++iter)
            {
                result /= *iter;
            }
        }
        return path{result.generic_string()};
        */
        return p.lexically_normal();
    }
    //---------------------------------------------------------------------------------------------------------------------
    //#####################################################################################################################
}
