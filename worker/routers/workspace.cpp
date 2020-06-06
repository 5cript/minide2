#include "workspace.hpp"

#include "../routers.hpp"
#include "../workspace/workspace.hpp"

#include "../json.hpp"
#include "../filesystem/directory_cache.hpp"

#include "../streaming/common_messages/binary_data.hpp"
#include "../workspace/stream_messages/directory_contents.hpp"

using namespace std::string_literals;

namespace Routers
{
//#####################################################################################################################
    struct Workspace::Implementation
    {
        WorkspaceInfo info;
    };
//#####################################################################################################################
    Workspace::Workspace(RouterCollection* collection, attender::tcp_server& server)
        : BasicRouter{collection}
        , impl_{new Workspace::Implementation{}}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    Workspace::~Workspace() = default;
//---------------------------------------------------------------------------------------------------------------------
    void Workspace::registerRoutes(attender::tcp_server& server)
    {
        /**
         *  Opens the workspace and makes a flat scan
         */
        server.post("/api/workspace/open", [this](auto req, auto res)
        {
            readJsonBody(req, res, [this, res, req](json const& body)
            {
                if (!body.contains("path"))
                    return res->status(400).send("need path in json body");

                auto root = body["path"].get<std::string>();

                if (!sfs::exists(root))
                    return res->status(400).send("directory does not exist");

                if (!body.contains("id"))
                    return res->status(400).send("body requires listener id");

                auto dir = std::make_unique <Streaming::Messages::DirectoryContent>(root);
                dir->scan(false, "");
                dir->origin = "/"s + sfs::path{root}.filename().string();
                auto result = collection_->streamer().send
                (
                    StreamChannel::Data,
                    req->ip(),
                    body["id"].get<int>(),
                    dir.release()
                );

                if (result != 0)
                    return res->status(400).send("please first listen to the data stream or provide correct listener id");

                impl_->info.root = root;
                res->status(204).end();
            });
        });

        /**
         *  Observe workspace directory
         */
        server.post("/api/workspace/observe", [this](auto req, auto res)
        {
            readJsonBody(req, res, [this, req, res](json const& body)
            {
                res->status(500).send("observer feature pending");
            });
        });


        /**
         *  Unobserve workspace directory
         */
        server.post("/api/workspace/unobserve", [this](auto req, auto res)
        {
            readJsonBody(req, res, [=](json const& body)
            {
                res->status(500).send("observer feature pending");
            });

        });

        /**
         *  List directories and files in a relative directory
         */
        server.post("/api/workspace/enlist", [this](auto req, auto res)
        {
            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("path"))
                    return res->status(400).send("need path in json body");

                auto recursive = false;
                if (body.contains("recursive") && body["recursive"].get<bool>())
                    recursive = true;

                if (impl_->info.root.empty())
                    return res->status(400).send("first open a workspace");

                if (!body.contains("id"))
                    return res->status(400).send("body requires listener id");

                std::string path = body["path"].get<std::string>();
                if (path.empty())
                    return res->status(400).send("path cannot be empty");

                if (path.find("//") != std::string::npos)
                    return res->status(400).send("not accepting double slashes in paths");

                if (path[0] == '/')
                    path = path.substr(1, path.size() - 1);

                auto relativePath = sfs::path{impl_->info.root}.remove_filename() / path;

                if (!sfs::exists(relativePath))
                    return res->status(400).send("directory does not exist");

                auto dir = std::make_unique <Streaming::Messages::DirectoryContent>(relativePath);
                dir->scan(recursive, "");
                dir->origin = "/"s + path;
                auto result = collection_->streamer().send
                (
                    StreamChannel::Data,
                    req->ip(),
                    body["id"].get<int>(),
                    dir.release()
                );

                if (result != 0)
                    return res->status(400).send("please first listen to the data stream or provide correct listener id");
                res->status(200).end();
            });
        });

        server.post("/api/workspace/project/activate", [this](auto req, auto res)
        {
            if (impl_->info.root.empty())
                return res->status(400).send("open a workspace first");

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("project"))
                    return res->status(400).send("need 'project' in json body.");

                impl_->info.activeProject = body["project"].get<std::string>();

                res->status(500).end();
            });
        });

        server.get("/api/workspace/info", [this](auto req, auto res)
        {
            res->send("thank you");
        });
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
