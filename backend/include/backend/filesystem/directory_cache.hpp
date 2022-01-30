#pragma once

#include "filesystem.hpp"

#include <string>
#include <memory>
#include <functional>

namespace Filesystem
{
    class DirectoryCache;

    struct CacheUpdateObserver
    {
        std::function<void(DirectoryCache const& cache, std::string const& name)> onAdd = [](DirectoryCache const&,
                                                                                             std::string const&) {};
        std::function<void(DirectoryCache const& cache, std::string const& name)> onRemove = [](DirectoryCache const&,
                                                                                                std::string const&) {};
        std::function<void(DirectoryCache const& cache, std::string const& name)> onModify = [](DirectoryCache const&,
                                                                                                std::string const&) {};
        std::function<void(DirectoryCache const& cache, std::string const& old, std::string const& fresh)> onRename =
            [](DirectoryCache const&, std::string const&, std::string const&) {};
    };

    class DirectoryCache
    {
      public:
        /**
         *  Caches directory contents and watches a directory for changes.
         *  @param dir the directory to watch (not recursive)
         *  @param observer An optional set of observers
         *  @param scan Scan the directory initially? If not will only track changes and cache those only.
         */
        DirectoryCache(
            Filesystem::path const& dir,
            CacheUpdateObserver observer = {},
            bool scan = true,
            bool saveChanges = true);
        ~DirectoryCache();

      private:
        void onAdd(std::string const& path);
        void onRemove(std::string const& path);
        void onModify(std::string const& path);
        void onRenamedOld(std::string const& path);
        void onRenamedNew(std::string const& path);

        void fullscan();

      private:
        struct Implementation;
        std::unique_ptr<Implementation> impl_;
    };

}
