#include "workspace.hpp"

#include "../routers.hpp"
#include "../workspace/workspace.hpp"

#include "../json.hpp"
#include "../hybrid_read_sink.hpp"
#include "../filesystem/directory_cache.hpp"

#include "../streaming/common_messages/binary_data.hpp"
#include "../workspace/stream_messages/directory_contents.hpp"
#include "../workspace/stream_messages/file_content.hpp"
#include "../workspace/hashed_file.hpp"

#include <string>
#include <fstream>

using namespace std::string_literals;

namespace Routers
{
//#####################################################################################################################
    struct LeakTest
    {
        ~LeakTest()
        {
            std::cout << "~LeakTest()\n";
        };
    };
//#####################################################################################################################
    struct Workspace::Implementation
    {
        Config config;

        Implementation(Config config)
            : config{std::move(config)}
        {
        }
    };
//#####################################################################################################################
    Workspace::Workspace(RouterCollection* collection, attender::tcp_server& server, Config const& config)
        : BasicRouter{collection, &server}
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
        cors_options(server, "/api/workspace/open", "POST", impl_->config.corsOption);
        server.post("/api/workspace/open", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            std::cout << req->get_cookie_value("aSID").value();
            auto sess = this_session(req);
            sess.dump();

            readJsonBody(req, res, [this, res, req](json const& body)
            {
                auto sess = this_session(req);

                if (!body.contains("path"))
                    return res->status(400).send("need path in json body");

                auto root = body["path"].get<std::string>();

                if (!sfs::exists(root))
                    return res->status(400).send("directory does not exist");

                auto id = sess.dataId;
                if (id == -1)
                    return res->status(400).send("please listen to the data stream before this request");

                auto dir = std::make_unique <Streaming::Messages::DirectoryContent>(root);
                dir->scan(false, "");
                dir->origin = "/"s + sfs::path{root}.filename().string();
                auto result = collection_->streamer().send
                (
                    StreamChannel::Data,
                    req->ip(),
                    id,
                    dir.release()
                );

                if (result != 0)
                    return res->status(400).send("please first listen to the data stream or provide correct listener id");

                sess.workspace.root = root;
                res->status(204).end();
            });
        });

        /**
         *  Observe workspace directory
         */
        cors_options(server, "/api/workspace/observe", "POST", impl_->config.corsOption);
        server.post("/api/workspace/observe", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                res->status(500).send("observer feature pending");
            });
        });


        /**
         *  Unobserve workspace directory
         */
        cors_options(server, "/api/workspace/unobserve", "POST", impl_->config.corsOption);
        server.post("/api/workspace/unobserve", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [=](json const& body)
            {
                res->status(500).send("observer feature pending");
            });

        });

        /**
         *  List directories and files in a relative directory
         */
        cors_options(server, "/api/workspace/enlist", "POST", impl_->config.corsOption);
        server.post("/api/workspace/enlist", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                auto sess = this_session(req);

                if (!body.contains("path"))
                    return res->status(400).send("need path in json body");

                auto recursive = false;
                if (body.contains("recursive") && body["recursive"].get<bool>())
                    recursive = true;

                if (sess.workspace.root.empty())
                    return res->status(400).send("first open a workspace");

                auto id = sess.dataId;
                if (id == -1)
                    return res->status(400).send("please listen to the data stream before this request");

                std::string path = body["path"].get<std::string>();

                auto veri = verifyPath(path, sess.workspace.root);
                if (!veri.first.empty())
                    return res->status(400).send(veri.first);

                auto dir = std::make_unique <Streaming::Messages::DirectoryContent>(veri.second);
                dir->scan(recursive, "");
                dir->origin = path;
                auto result = collection_->streamer().send
                (
                    StreamChannel::Data,
                    req->ip(),
                    id,
                    dir.release()
                );

                if (result != 0)
                    return res->status(400).send("please first listen to the data stream or provide correct listener id");
                res->status(200).end();
            });
        });

        cors_options(server, "/api/workspace/project/activate", "POST", impl_->config.corsOption);
        server.post("/api/workspace/project/activate", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto sess = this_session(req);
            if (sess.workspace.root.empty())
                return res->status(400).send("open a workspace first");

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("project"))
                    return res->status(400).send("need 'project' in json body.");

                auto sess = this_session(req);
                sess.workspace.activeProject = body["project"].get<std::string>();

                res->status(500).end();
            });
        });

        /**
         *  Get Information about the workspace.
         */
        cors_options(server, "/api/workspace/info", "GET", impl_->config.corsOption);
        server.get("/api/workspace/info", [this, &server](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto sess = this_session(req);
            std::cout << sess.controlId << "\n";
            std::cout << sess.dataId << "\n";
            std::cout << sess.workspace.root << "\n";
            std::cout << "---\n";

            res->status(200).send("ok dokey");
        });

        cors_options(server, "/api/workspace/loadFile", "POST", impl_->config.corsOption);
        server.post("/api/workspace/loadFile", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto sess = this_session(req);
            if (sess.workspace.root.empty())
                return res->status(400).send("open a workspace first");

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("path"))
                    return res->status(400).send("need path in json body");

                auto sess = this_session(req);
                auto id = sess.dataId;
                if (id == -1)
                    return res->status(400).send("please listen to the data stream before this request");

                std::string path = body["path"].get<std::string>();

                bool force = false;
                if (body.contains("force"))
                    force = body["force"].get<bool>();

                auto veri = verifyPath(path, sess.workspace.root);
                if (!veri.first.empty())
                    return res->status(400).send(veri.first);

                if (!sfs::is_regular_file(veri.second))
                    return res->status(400).send("is not a regular file");

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
                    id,
                    fc.release()
                );

                if (result != 0)
                    return res->status(400).send("please first listen to the data stream or provide correct listener id");
                res->status(200).end();
            });
        });

        cors_options(server, "/api/workspace/saveFile", "PUT", impl_->config.corsOption);
        server.put("/api/workspace/saveFile", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto observer = res->observe_conclusion();

            auto sess = this_session(req);

            if (sess.workspace.root.empty())
                return res->status(400).send("open a workspace first");

            auto maybeContentLen = req->get_header_field("Content-Length");
            long long len = 0;
            if (maybeContentLen)
            {
                try
                {
                    len = std::stoll(maybeContentLen.value());
                }
                catch (std::exception const& exc)
                {
                    // conversion failed!
                    return res->status(400).send(exc.what());
                }
            }
            else
                return res->status(411).end();

            if (len > impl_->config.maxFileWriteSize)
                return res->status(413).end();

            if (len < 13)
                return res->status(400).send("Content-Length is too small to be correct (10 bytes hex json len + | + json + data)");

            std::shared_ptr <HashedFile> file = std::make_shared <HashedFile>();
            std::shared_ptr <JsonDataHybridSink> sink{std::make_shared <JsonDataHybridSink>(
                // json complete
                [file, this, res, req](json const& jsonPart)
                {
                    try
                    {
                        if (!jsonPart.contains("path"))
                            return res->status(400).send("need path in json body part");

                        if (!jsonPart.contains("sha256"))
                            return res->status(400).send("requires a sha256 in the json body for verification");

                        auto hash = jsonPart["sha256"].get<std::string>();

                        auto sess = this_session(req);
                        auto const [error, path] = verifyPath(jsonPart["path"].get<std::string>(), sess.workspace.root, false);
                        if (!error.empty())
                            return res->status(400).send(error);

                        // FIXME: dont save in same dir??
                        file->open(path, hash);
                        if (!file->good())
                            return res->status(400).send("cannot open file");
                    }
                    catch(std::exception const& exc)
                    {
                        res->status(500).send(exc.what());
                    }
                },
                // on data
                [file, res](char const* data, std::size_t amount)
                {
                    try
                    {
                        if (!file->good())
                            return;

                        file->write(data, amount);
                    }
                    catch(std::exception const& exc)
                    {
                        res->status(500).send(exc.what());
                    }
                },
                // expectation failure
                [res](std::string const& msg)
                {
                    res->status(400).send(msg);
                },
                static_cast <std::size_t> (len)
            )};

            //std::cout.put('a');

            if (observer->has_concluded())
                return;

            req->read_body(sink).then([sink, file, res, observer]()
            {
                //std::cout.put('b');
                try
                {
                    auto success = file->testAndMove();

                    //std::cout.put('c');

                    // only execute this when no other send exit path was taken
                    if (observer->has_concluded())
                        return;

                    //std::cout.put('d');

                    if (success)
                        res->status(204).end();
                    else
                        res->status(500).send("transfered file does not match sent hash.");

                    //std::cout.put('e');
                }
                catch(...)
                {
                    //std::cout.put('f');
                    if (observer->has_concluded())
                        return;
                    //std::cout.put('g');
                    res->status(500).end();
                }
            });
        });

        cors_options(server, "/api/workspace/setActiveProject", "POST", impl_->config.corsOption);
        server.post("/api/workspace/setActiveProject", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto sess = this_session(req);
            if (sess.workspace.root.empty())
                return res->status(400).send("open a workspace first");

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("path"))
                    return res->status(400).send("need path in json body");

                std::string path = body["path"].get<std::string>();

                auto sess = this_session(req);
                auto veri = verifyPath(path, sess.workspace.root);
                if (!veri.first.empty())
                    return res->status(400).send(veri.first);

                if (!sfs::is_directory(veri.second))
                    return res->status(400).send("is not a directory");

                sess.workspace.activeProject = veri.second;

                res->status(200).end();
            });
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    std::pair <std::string, std::string> Workspace::verifyPath(std::string path, std::string const& root, bool mustExist)
    {
        if (path.empty())
            return {"path cannot be empty"s, path};

        if (path.find("//") != std::string::npos)
            return {"not accepting double slashes in paths"s, path};

        if (path[0] == '/')
            path = path.substr(1, path.size() - 1);

        auto relativePath = sfs::path{root}.remove_filename() / path;

        if (mustExist && !sfs::exists(relativePath))
            return {"path does not exist"s, path};

        return {""s, relativePath.string()};
    }
//#####################################################################################################################
}
