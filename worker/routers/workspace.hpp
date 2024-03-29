#pragma once

#include "router_base.hpp"
#include "../config.hpp"
#include "../filesystem/filesystem.hpp"

#include <memory>
#include <string>
#include <utility>

namespace Routers
{
    class Workspace : public BasicRouter
    {
    public:
        Workspace(CommunicationCenter* collection, attender::http_server& server, Config const& config);
        ~Workspace();

    private:
        void registerRoutes(attender::http_server& server);

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
        struct Implementation;
        std::unique_ptr <Implementation> impl_;
    };
}
