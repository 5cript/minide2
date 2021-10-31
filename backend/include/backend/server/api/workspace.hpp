#pragma once

#include <backend/server/stream/dispatcher.hpp>
#include <backend/server/api/api_base.hpp>
#include <backend/json.hpp>
#include <backend/filesystem/filesystem.hpp>

#include <tuple>
#include <string>

namespace Api
{
    class Workspace : public ApiBase
    {
    public:
        DECLARE_API(Workspace)

        /**
         *  Returns an pair of <E, P>. There was an error if E is non-empty
         */
        std::tuple <std::string, std::string, json> verifyPath(std::string path, sfs::path const& root, bool mustExist = true, bool enforceWorkspaceRelative = false);

        std::tuple <
            sfs::path, // include/source path file (empty, if original is not within a include/source directory)
            sfs::path, // inplace path
            bool, // include/source directory exists
            bool // file is in include/source directory
        >
        toggleSourceHeader(sfs::path const& original, bool isSource, std::string const& targetExtension);
    
    private:
        void doSubscribe();
        void open(int ref, sfs::path const& root);
        void enlist(int ref, sfs::path const& path, bool recursive);
        void loadFile(int ref, sfs::path const& path);

    private:
        sfs::path root_;
    };
}