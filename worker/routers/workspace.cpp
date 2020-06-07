#include "workspace.hpp"

#include "../routers.hpp"
#include "../workspace/workspace.hpp"

#include "../json.hpp"
#include "../filesystem/directory_cache.hpp"

#include "../streaming/common_messages/binary_data.hpp"
#include "../workspace/stream_messages/directory_contents.hpp"
#include "../workspace/stream_messages/file_content.hpp"

using namespace std::string_literals;

namespace Routers
{
//#####################################################################################################################
    struct Workspace::Implementation
    {
        WorkspaceInfo info;
        Config config;

        Implementation(Config config)
            : info{}
            , config{std::move(config)}
        {
        }
    };
//#####################################################################################################################
    Workspace::Workspace(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter{collection}
        , impl_{new Workspace::Implementation(config)}
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
        cors_options(server, "/api/workspace/open", "POST");
        server.post("/api/workspace/open", [this](auto req, auto res)
        {
            enable_cors(res);

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
        cors_options(server, "/api/workspace/observe", "POST");
        server.post("/api/workspace/observe", [this](auto req, auto res)
        {
            enable_cors(res);

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                res->status(500).send("observer feature pending");
            });
        });


        /**
         *  Unobserve workspace directory
         */
        cors_options(server, "/api/workspace/unobserve", "POST");
        server.post("/api/workspace/unobserve", [this](auto req, auto res)
        {
            enable_cors(res);

            readJsonBody(req, res, [=](json const& body)
            {
                res->status(500).send("observer feature pending");
            });

        });

        /**
         *  List directories and files in a relative directory
         */
        cors_options(server, "/api/workspace/enlist", "POST");
        server.post("/api/workspace/enlist", [this](auto req, auto res)
        {
            enable_cors(res);

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

                auto veri = verifyPath(path);
                if (!veri.first.empty())
                    return res->status(400).send(veri.first);

                auto dir = std::make_unique <Streaming::Messages::DirectoryContent>(veri.second);
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

        cors_options(server, "/api/workspace/project/activate", "POST");
        server.post("/api/workspace/project/activate", [this](auto req, auto res)
        {
            enable_cors(res);

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

        /**
         *  Get Information about the workspace.
         */
        cors_options(server, "/api/workspace/info", "GET");
        server.get("/api/workspace/info", [this](auto req, auto res)
        {
            enable_cors(res);

            res->send("thank you");
        });

        cors_options(server, "/api/workspace/loadFile", "POST");
        server.post("/api/workspace/loadFile", [this](auto req, auto res)
        {
            enable_cors(res);

            if (impl_->info.root.empty())
                return res->status(400).send("open a workspace first");

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("path"))
                    return res->status(400).send("need path in json body");

                if (!body.contains("id"))
                    return res->status(400).send("body requires listener id");
                std::string path = body["path"].get<std::string>();

                bool force = false;
                if (body.contains("force"))
                    force = body["force"].get<bool>();

                auto veri = verifyPath(path);
                if (!veri.first.empty())
                    return res->status(400).send(veri.first);

                auto fc = std::make_unique <Streaming::Messages::FileContent>();
                fc->load
                (
                    veri.second,
                    force,
                    impl_->config.maxFileReadSize,
                    impl_->config.maxFileReadSizeUnforceable,
                    impl_->config.fileChunkSize
                );
                fc->path = path;

                auto result = collection_->streamer().send
                (
                    StreamChannel::Data,
                    req->ip(),
                    body["id"].get<int>(),
                    fc.release()
                );

                if (result != 0)
                    return res->status(400).send("please first listen to the data stream or provide correct listener id");
                res->status(200).end();
            });
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    std::pair <std::string, std::string> Workspace::verifyPath(std::string path)
    {
        if (path.empty())
            return {"path cannot be empty"s, path};

        if (path.find("//") != std::string::npos)
            return {"not accepting double slashes in paths"s, path};

        if (path[0] == '/')
            path = path.substr(1, path.size() - 1);

        auto relativePath = sfs::path{impl_->info.root}.remove_filename() / path;

        if (!sfs::exists(relativePath))
            return {"path does not exist"s, path};

        return {""s, relativePath.string()};
    }
//#####################################################################################################################
}
