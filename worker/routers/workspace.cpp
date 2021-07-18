#include "workspace.hpp"

#include "../communication_center.hpp"
#include "../workspace/workspace.hpp"

#include "../json.hpp"
#include "../hybrid_read_sink.hpp"
#include "../filesystem/directory_cache.hpp"
#include "../filesystem/relations.hpp"

#include "../streaming/common_messages/binary_data.hpp"
#include "../workspace/stream_messages/directory_contents.hpp"
#include "../workspace/stream_messages/file_content.hpp"
#include "../workspace/hashed_file.hpp"
#include "../workspace/workspace_persistence.hpp"
#include "../workspace/project_persistence.hpp"


#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <type_traits>

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
    Workspace::Workspace(CommunicationCenter* collection, attender::http_server& server, Config const& config)
        : BasicRouter{collection, &server}
        , impl_{new Workspace::Implementation(config)}
    {
        registerRoutes(server);
    }
//---------------------------------------------------------------------------------------------------------------------
    Workspace::~Workspace() = default;
//---------------------------------------------------------------------------------------------------------------------
    void Workspace::registerRoutes(attender::http_server& server)
    {
        /**
         *  Opens the workspace and makes a flat scan
         */
        cors_options(server, "/api/workspace/open", "POST", impl_->config.corsOption);
        server.post("/api/workspace/open", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            readJsonBody(req, res, [this, res, req](json const& body)
            {
                auto sess = this_session(req);

                if (!body.contains("path"))
                    return respondWithError(res, "need path in json body");

                auto root = body["path"].get<std::string>();

                if (!sfs::exists(root))
                    return respondWithError(res, "directory does not exist");

                auto id = sess.dataId;
                if (id == -1)
                    return respondWithError(res, "please listen to the data stream before this request");

                auto dir = std::make_unique <Streaming::Messages::DirectoryContent>(root);
                dir->scan(false, "");
                dir->origin = "/"s + sfs::path{root}.filename().string();
                auto result = collection_->streamer().send
                (
                    Streaming::StreamChannel::Data,
                    req->ip(),
                    id,
                    dir.release()
                );

                if (result != 0)
                    return respondWithError(res, "please first listen to the data stream or provide correct listener id");

                sess.workspace.root = root;
                sess.save_partial([](auto& toSave, auto& from)
                {
                    toSave.workspace.root = from.workspace.root;
                });
                auto sess2 = this_session(req);
                sess2.dump();
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
                    return respondWithError(res, "need path in json body");

                auto recursive = false;
                if (body.contains("recursive") && body["recursive"].get<bool>())
                    recursive = true;

                if (sess.workspace.root.empty())
                    return respondWithError(res, "first open a workspace");

                auto id = sess.dataId;
                if (id == -1)
                    return respondWithError(res, "please listen to the data stream before this request");

                std::string path = body["path"].get<std::string>();

                auto veri = verifyPath(path, sess.workspace.root);
                if (!std::get<0>(veri).empty())
                    return respondWithError(res, std::get<0>(veri), std::get<2>(veri));

                auto dir = std::make_unique <Streaming::Messages::DirectoryContent>(std::get<1>(veri));
                dir->scan(recursive, "");
                dir->origin = path;
                auto result = collection_->streamer().send
                (
                    Streaming::StreamChannel::Data,
                    req->ip(),
                    id,
                    dir.release()
                );

                if (result != 0)
                    return respondWithError(res, "please first listen to the data stream or provide correct listener id");
                res->status(200).end();
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

        cors_options(server, "/api/workspace/loadWorkspaceMeta", "GET", impl_->config.corsOption);
        server.get("/api/workspace/loadWorkspaceMeta", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto sess = this_session(req);
            if (sess.workspace.root.empty())
                return respondWithError(res, "open a workspace first");

            WorkspacePersistence wspace{sess.workspace.root};
            try
            {
                wspace.load();
                res->status(200).send(wspace.raw());
            }
            catch(std::exception const& exc)
            {
                return res->status(500).send(exc.what());
            }
        });

        cors_options(server, "/api/workspace/loadProjectMeta", "GET", impl_->config.corsOption);
        server.get("/api/workspace/loadProjectMeta", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto sess = this_session(req);
            if (sess.workspace.root.empty())
                return respondWithError(res, "open a workspace first");

            if (sess.workspace.activeProject.empty())
                return respondWithError(res, "no active project");

            ProjectPersistence project{sess.workspace.activeProject};
            try
            {
                project.load();
                auto raw = project.raw();
                if (raw.empty())
                    res->status(200).send("{}");
                else
                    res->status(200).send(project.raw());
            }
            catch(std::exception const& exc)
            {
                return res->status(500).send(exc.what());
            }
        });

        cors_options(server, "/api/workspace/changeProjectMeta", "POST", impl_->config.corsOption);
        server.post("/api/workspace/changeProjectMeta", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            {
                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");

                if (sess.workspace.activeProject.empty())
                    return respondWithError(res, "no active project");
            }

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");

                if (sess.workspace.activeProject.empty())
                    return respondWithError(res, "no active project");

                ProjectPersistence project{sess.workspace.activeProject};
                project.load();
                project.inject(body);
                project.save();

                res->status(200).end();
            });
        });

        cors_options(server, "/api/workspace/deleteFile", "POST", impl_->config.corsOption);
        server.post("/api/workspace/deleteFile", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            {
                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");
            }

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("path"))
                    return respondWithError(res, "need path in json body");

                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");

                std::string path = body["path"].get<std::string>();
                auto veri = verifyPath(path, sess.workspace.root, true, true);
                if (!std::get<0>(veri).empty())
                    return respondWithError(res, std::get<0>(veri), std::get<2>(veri));

                std::cout << "ordered to delete: " << std::get<1>(veri) << "\n";

                return res->status(200).end();
            });
        });

        cors_options(server, "/api/workspace/loadFile", "POST", impl_->config.corsOption);
        server.post("/api/workspace/loadFile", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            {
                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");
            }

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("path"))
                    return respondWithError(res, "need path in json body");

                auto sess = this_session(req);
                auto id = sess.dataId;
                if (id == -1)
                    return respondWithError(res, "please listen to the data stream before this request");

                std::string path = body["path"].get<std::string>();

                bool force = false;
                if (body.contains("force"))
                    force = body["force"].get<bool>();

                auto veri = verifyPath(path, sess.workspace.root);
                if (!std::get<0>(veri).empty())
                    return respondWithError(res, std::get<0>(veri), std::get<2>(veri));

                if (!sfs::is_regular_file(std::get<1>(veri)))
                    return respondWithError(res, "is not a regular file");

                auto fc = std::make_unique <Streaming::Messages::FileContent>();
                fc->load
                (
                    std::get<1>(veri),
                    force,
                    impl_->config.maxFileReadSize,
                    impl_->config.maxFileReadSizeUnforceable,
                    impl_->config.fileChunkSize
                );
                fc->path = path;

                if (body.contains("flag"))
                    fc->flag = body["flag"].get<std::string>();

                auto result = collection_->streamer().send
                (
                    Streaming::StreamChannel::Data,
                    req->ip(),
                    id,
                    fc.release()
                );

                if (result != 0)
                    return respondWithError(res, "please first listen to the data stream or provide correct listener id");
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
                return respondWithError(res, "open a workspace first");

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
                    return respondWithError(res, exc.what());
                }
            }
            else
                return res->status(411).end();

            if (len > impl_->config.maxFileWriteSize)
                return res->status(413).end();

            if (len < 13)
                return respondWithError(res, "Content-Length is too small to be correct (10 bytes hex json len + | + json + data)");

            std::shared_ptr <HashedFile> file = std::make_shared <HashedFile>();
            std::shared_ptr <JsonDataHybridSink> sink{std::make_shared <JsonDataHybridSink>(
                // json complete
                [file, this, res, req](json const& jsonPart)
                {
                    try
                    {
                        if (!jsonPart.contains("path"))
                            return respondWithError(res, "need path in json body part");

                        if (!jsonPart.contains("sha256"))
                            return respondWithError(res, "requires a sha256 in the json body for verification");

                        auto hash = jsonPart["sha256"].get<std::string>();

                        auto sess = this_session(req);
                        auto const [error, path, j] = verifyPath(jsonPart["path"].get<std::string>(), sess.workspace.root, false);
                        if (!error.empty())
                            return respondWithError(res, error, j);

                        // FIXME: dont save in same dir??
                        file->open(path, hash);
                        if (!file->good())
                            return respondWithError(res, "cannot open file");
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
                [res, this](std::string const& msg)
                {
                    respondWithError(res, msg);
                },
                static_cast <std::size_t> (len)
            )};

            if (observer->has_concluded())
            {
                std::cout << "hasConcluded\n";
                return;
            }

            req->read_body(sink).then([sink, file, res, observer]()
            {
                try
                {
                    auto success = file->testAndMove();

                    // only execute this when no other send exit path was taken
                    if (observer->has_concluded())
                        return;

                    if (success)
                        res->status(204).end();
                    else
                        res->status(500).send("transfered file does not match sent hash.");
                }
                catch(...)
                {
                    if (observer->has_concluded())
                        return;
                    res->status(500).end();
                }
            })
            .except([](...)
            {
                std::cout << "huh?\n";
            });
        });

        cors_options(server, "/api/workspace/setActiveProject", "POST", impl_->config.corsOption);
        server.post("/api/workspace/setActiveProject", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            {
                auto sess = this_session(req);
                sess.dump();
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");
            }

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("path"))
                    return respondWithError(res, "need path in json body");

                std::string path = body["path"].get<std::string>();

                auto sess = this_session(req);
                auto veri = verifyPath(path, sess.workspace.root);
                if (!std::get<0>(veri).empty())
                    return respondWithError(res, std::get<0>(veri), std::get<2>(veri));

                if (!sfs::is_directory(std::get<1>(veri)))
                    return respondWithError(res, "is not a directory");

                sess.workspace.activeProject = std::get<1>(veri);
                sess.save_partial([](auto& toSave, auto& from)
                {
                    toSave.workspace.activeProject = from.workspace.activeProject;
                });

                WorkspacePersistence wspace{sess.workspace.root};
                wspace.load();
                wspace.lastActiveProject = sess.workspace.activeProject.string();
                wspace.save();
                std::cout << "Opened workspace: " << sess.workspace.activeProject << "\n";

                res->status(200).end();
            });
        });

        cors_options(server, "/api/workspace/toggleSourceHeader", "POST", impl_->config.corsOption);
        server.post("/api/workspace/toggleSourceHeader", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            {
                auto sess = this_session(req);
                if (sess.workspace.root.empty())
                    return respondWithError(res, "open a workspace first");
            }

            readJsonBody(req, res, [this, req, res](json const& body)
            {
                if (!body.contains("path"))
                    return respondWithError(res, "need path in json body");

                auto sess = this_session(req);
                auto id = sess.controlId;
                if (id == -1)
                    return respondWithError(res, "please listen to the control stream before this request");

                std::string sourceExtension = "auto";
                std::string headerExtension = "auto";
                if (body.contains("sourceExtension"))
                    sourceExtension = body["sourceExtension"].get<std::string>();
                if (body.contains("headerExtension"))
                    sourceExtension = body["headerExtension"].get<std::string>();
                std::string targetExtension;

                std::string path = body["path"].get<std::string>();

                auto original = sfs::path{path};
                auto extension = original.extension().string();

                // I am shredding information here that is "encoded" where capital letters are c++ files.
                // bullshit denomination to me, so eff it.
                std::transform(std::begin(extension), std::end(extension), std::begin(extension), [](auto c){return std::tolower(c);});
                bool isSource = false;
                if
                (
                    extension == ".cpp" ||
                    extension == ".c" ||
                    extension == ".cxx" ||
                    extension == ".cc" ||
                    extension == ".c++" ||
                    extension == ".cp" ||
                    extension == ".ii" ||
                    extension == ".i" ||
                    extension == ".m" ||
                    extension == ".mi" ||
                    extension == ".mm" ||
                    extension == ".mii"
                )
                {
                    if (headerExtension != "auto")
                        targetExtension = headerExtension;
                    else
                    {
                        if (extension[1] == 'c')
                            targetExtension = ".h"s + extension.substr(2, extension.length() - 2);
                        else
                            // this is easier, but not great. because there is distinction between cpp and c in
                            // the other quirky extensions. but its not really worth the effort here.
                            targetExtension = ".h";

                    }
                    isSource = true;
                }
                else if //is header
                (
                    extension == ".hh" ||
                    extension == ".h" ||
                    extension == ".hp" ||
                    extension == ".hpp" ||
                    extension == ".hxx" ||
                    extension == ".h++" ||
                    extension == ".tcc" ||
                    extension == ".txx"
                )
                {
                    if (sourceExtension != "auto")
                        targetExtension = sourceExtension;
                    else
                    {
                        if (extension[1] == 'h')
                            targetExtension = ".c"s + extension.substr(2, extension.length() - 2);
                        else
                            targetExtension = ".cpp"; // tcc and txx are c++ extension
                    }
                    isSource = false;
                }
                else
                {
                    return respondWithError(res, "File is not recognized as a C/C++/Objective-C source or header file by extension");
                }

                auto findResult = toggleSourceHeader(original, isSource, targetExtension);

                json j = json::object();
                j["specialDirExists"] = std::get<2>(findResult);
                Filesystem::Jail jail(sess.workspace.root);
                auto makeRelative = [&jail, &findResult](auto dummy)
                {
                    auto path = std::get<decltype(dummy)::value>(findResult);
                    if (path.empty())
                        return path;
                    auto maybeRelative = jail.relativeToRoot(path, true);
                    if (maybeRelative)
                        return maybeRelative.value();
                    return path;
                };
                j["specialDirFile"] = makeRelative(std::integral_constant <int, 0>{}).generic_string();
                j["inplaceFile"] = makeRelative(std::integral_constant <int, 1>{}).generic_string();
                j["fileInSpecialDir"] = std::get<3>(findResult);

                auto result = collection_->streamer().send
                (
                    Streaming::StreamChannel::Control,
                    req->ip(),
                    id,
                    j,
                    "toggle_source_header"
                );

                if (result != 0)
                    return respondWithError(res, "please first listen to the data stream or provide correct listener id");
                res->status(200).end();
            });
        });

        cors_options(server, "/api/workspace/getRunConfigs", "GET", impl_->config.corsOption);
        server.get("/api/workspace/getRunConfigs", [this](auto req, auto res)
        {
            enable_cors(req, res, impl_->config.corsOption);

            auto sess = this_session(req);
            if (sess.workspace.root.empty())
                return respondWithError(res, "open a workspace first");
            if (sess.workspace.activeProject.empty())
                return respondWithError(res, "set an active project");

            const auto runJsonFile = sess.workspace.activeProject / ".minIDE" / "run.json";
            if (!sfs::exists(runJsonFile))
                return res->send("{}");
            if (!res->send_file(runJsonFile.string()))
                return respondWithError(res, 404, "cannot open file or read file");
            else
                return;
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    std::tuple <
        sfs::path,
        sfs::path,
        bool,
        bool
    >
    Workspace::toggleSourceHeader(sfs::path const& original, bool isSource, std::string const& targetExtension)
    {
        //sfs::path original{"/test/project/source/something/a/file.cpp"};
        auto reduced = original.parent_path();

        std::deque <std::string> parts;
        for (auto i = std::begin(reduced); i != std::end(reduced); ++i)
        {
            parts.push_front(i->string());
        }

        bool anyFound = false;
        for (auto const& i : parts)
        {
            if (isSource && (i == "source" || i == "sources" || i == "src"))
            {
                anyFound = true;
                break;
            }
            else if (!isSource && (i == "include" || i == "headers"))
            {
                anyFound = true;
                break;
            }
            else
                reduced = reduced.parent_path();
        }

        if (!anyFound)
            reduced = sfs::path{};

        sfs::path specialDirPartner;
        bool specialExists = false;
        if (!reduced.empty() && reduced.string() != "/")
        {
            auto relative = sfs::proximate(original.parent_path(), reduced);

            auto specialDir = sfs::path{};
        #define TEST_SPECIAL_DIR(X) \
            if (specialDir.empty() && sfs::exists(X)) \
                specialDir = X;

            if (isSource)
            {
                TEST_SPECIAL_DIR(reduced.parent_path() / "include");
                TEST_SPECIAL_DIR(reduced.parent_path() / "headers");

                if (specialDir.empty())
                    specialDir = reduced.parent_path() / "include";
                else
                    specialExists = true;
            }
            else
            {
                TEST_SPECIAL_DIR(reduced.parent_path() / "source");
                TEST_SPECIAL_DIR(reduced.parent_path() / "src");
                TEST_SPECIAL_DIR(reduced.parent_path() / "sources");

                if (specialDir.empty())
                    specialDir = reduced.parent_path() / "source";
                else
                    specialExists = true;
            }

            specialDirPartner = specialDir / relative / (original.stem().string() + targetExtension);
        }
        auto samePlacePartner = original.parent_path() / (original.stem().string() + targetExtension);

        return {
            specialDirPartner.generic_string(),
            samePlacePartner.generic_string(),
            specialExists,
            anyFound
        };
    }
//---------------------------------------------------------------------------------------------------------------------
    std::tuple <std::string, std::string, json> Workspace::verifyPath
    (
        std::string path,
        sfs::path const& root,
        bool mustExist,
        bool enforceWorkspaceRelative
    )
    {
        if (path.empty())
            return {"path cannot be empty"s, path, json::object()};

        if (path[0] == '/')
            path = path.substr(1, path.size() - 1);

        auto rootFilename = root.filename().string();
        sfs::path resultPath;
        if (path == rootFilename)
        {
            resultPath = root;
        }
        else if ((path.length() > rootFilename.length()) && path.substr(0, rootFilename.length()) == rootFilename)
        {
            Filesystem::Jail jail(root.parent_path());
            auto relative = jail.relativeToRoot(path, false);
            if (!relative && enforceWorkspaceRelative)
                return {"path is not within workspace, and thats forbidden"s, path, json::object(
                    {"pathNotInWorkspace", true}
                )};

            if (relative)
                resultPath = root.parent_path() / relative.value();
            else
                resultPath = path;
        }
        else if (enforceWorkspaceRelative)
        {
            return {"path is not within workspace, and thats forbidden"s, path, json::object(
                {"pathNotInWorkspace", true}
            )};
        }

        if (mustExist && !sfs::exists(resultPath))
            return {"path does not exist", path, json{
                {"fileNotFound", true},
                {"path", path}
            }};

        return {""s, resultPath.generic_string(), json::object()};
    }
//#####################################################################################################################
}
