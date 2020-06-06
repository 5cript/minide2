#pragma once

#include "../../filesystem/filesystem.hpp"
#include "../../json.hpp"

#include <optional>

namespace Streaming::Messages
{
    /**
     *  Used for sending the content of a directory (recursive).
     */
    class DirectoryContent : public JsonSerializable
    {
    public:
        sfs::path root;
        std::vector <std::string> files;
        std::vector <DirectoryContent> directories;
        std::optional <std::string> origin;
        bool flatDirectory;

        std::string toJson() const override;

        /**
         *  Does NOT automatically scan root dir.
         */
        DirectoryContent(sfs::path const& root);

        /**
         *  Use this to actually scan the directory
         */
        void scan(bool recursive, std::string const& rootReplace, int recursionLimit = 100);
    };
}
