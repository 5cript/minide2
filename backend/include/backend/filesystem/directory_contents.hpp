#pragma once

#include <backend/filesystem/filesystem.hpp>
#include <backend/json.hpp>

#include <optional>

namespace Filesystem
{
    /**
     *  Used for sending the content of a directory (recursive).
     */
    class DirectoryContent
    {
    public:
        sfs::path root;
        std::vector <std::string> files;
        std::vector <DirectoryContent> directories;
        std::optional <std::string> origin;
        bool flatDirectory;

        /**
         *  Does NOT automatically scan root dir.
         */
        DirectoryContent(sfs::path const& root);

        /**
         *  Use this to actually scan the directory
         */
        void scan(bool recursive, int recursionLimit = 100);
    };
    
    /**
     * 
     */
    static void to_json(json& j, DirectoryContent const& dc)
    {
        j["name"] = dc.root.string();
        if (dc.origin)
            j["origin"] = *dc.origin;
        if (!dc.files.empty())
            j["files"] = dc.files;
        if (!dc.flatDirectory)
        {
            j["flat"] = false;
            if (!dc.directories.empty())
                j["directories"] = dc.directories;
        }
        else
        {
            j["flat"] = true;
            std::vector <std::string> flat;
            for (auto const& d: dc.directories)
                flat.push_back(d.root.string());
            j["directories"] = flat;
        }
    }
//#####################################################################################################################
}
