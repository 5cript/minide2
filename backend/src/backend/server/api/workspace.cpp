#include <backend/server/api/workspace.hpp>
#include <backend/filesystem/directory_contents.hpp>
#include <backend/server/frontend_user_session.hpp>
#include <backend/json.hpp>
#include <backend/filesystem/jail.hpp>

#include <iostream>
#include <deque>

using namespace std::string_literals;

namespace Api
{
//#####################################################################################################################
    void Workspace::doSubscribe()
    {
        /*
        {
            payload: {
                path: string
            }
        }
        */
        subscribe("/api/workspace/open", [this](json const& j) {
            open(j["ref"].get<int>(), j["payload"]["path"].get<std::string>());
        });

        /*
        {
            payload: {
                path: string,
                recursive?: boolean
            }
        }
        */
        subscribe("/api/workspace/enlist", [this](json const& j) {
            enlist(
                j["ref"].get<int>(), 
                j["payload"]["path"].get<std::string>(), 
                j["payload"].contains("recursive") ? j["payload"]["recursive"].get<bool>() : false
            );
        });

        /*
        {
            payload: {
                path: string,
            }
        }
        */
        subscribe("/api/workspace/loadFile", [this](json const&  j) {
            loadFile(
                j["ref"].get<int>(), 
                j["payload"]["path"].get<std::string>()
            );
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void Workspace::open(int ref, sfs::path const& root)
    {
        auto sess = session();
        if (!sess)
            return;

        if (!sfs::exists(root))
            return sess->respondWithError(ref, "Directory does not exist '"s + root.string() + "'.");

        auto dir = Filesystem::DirectoryContent{root};
        dir.scan(false);
        dir.origin = "/"s + sfs::path{root}.filename().string();
        root_ = root;
        sess->writeJson({
            {"ref", ref},
            {"directory", dir}
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void Workspace::enlist(int ref, sfs::path const& path, bool recursive)
    {
        auto sess = session();
        if (!sess)
            return;

        if (root_.empty())
            return sess->respondWithError(ref, "First open a workspace.");

        auto veri = verifyPath(path.string(), root_);
        if (!std::get<0>(veri).empty())
            return sess->respondWithError(ref, std::get<0>(veri));

        auto dir = Filesystem::DirectoryContent(std::get<1>(veri));
        dir.scan(recursive);
        dir.origin = path.string();
        sess->writeJson({
            {"ref", ref},
            {"directory", dir}
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    void Workspace::loadFile(int ref, sfs::path const& path)
    {
        auto sess = session();
        if (!sess)
            return;

        if (root_.empty())
            return sess->respondWithError(ref, "First open a workspace.");
        auto veri = verifyPath(path.string(), root_);
        if (!std::get<0>(veri).empty())
            return sess->respondWithError(ref, std::get<0>(veri));

        // TODO:
        //sess->respondWithError(ref, "IMPLEMENT ME.");
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
            return {"Path cannot be empty."s, path, json::object()};

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
                return {"Path is not within workspace, and thats forbidden."s, path, json::object(
                    {"pathNotInWorkspace", true}
                )};

            if (relative)
                resultPath = root.parent_path() / relative.value();
            else
                resultPath = path;
        }
        else if (enforceWorkspaceRelative)
        {
            return {"Path is not within workspace, and thats forbidden."s, path, json::object(
                {"pathNotInWorkspace", true}
            )};
        }

        if (mustExist && !sfs::exists(resultPath))
            return {"Path does not exist.", path, json{
                {"fileNotFound", true},
                {"path", path}
            }};

        return {""s, resultPath.generic_string(), json::object()};
    }
//#####################################################################################################################
}