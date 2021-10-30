#include <backend/filesystem/directory_cache.hpp>

//#include <filewatch/FileWatch.hpp>

#include <functional>
#include <set>
#include <mutex>

namespace Filesystem
{
//#####################################################################################################################
    struct DirectoryCache::Implementation
    {
        Implementation
        (
            DirectoryCache& cache,
            Filesystem::path const& dir_,
            /*std::function <void(std::string const&, const filewatch::Event)> const& watch,*/
            CacheUpdateObserver observer,
            bool scan,
            bool saveChange
        );

        Filesystem::path dir;
        //std::unique_ptr <filewatch::FileWatch <std::wstring>> watcher;
        std::set <std::string> contents;
        std::string renameCache;
        std::mutex renameLock;
        CacheUpdateObserver observer;
        bool saveChange;
    };
//---------------------------------------------------------------------------------------------------------------------
    DirectoryCache::Implementation::Implementation
    (
        DirectoryCache& cache,
        Filesystem::path const& dir_,
        /*std::function <void(std::string const&, const filewatch::Event)> const& watch,*/
        CacheUpdateObserver observer,
        bool scan,
        bool saveChange
    )
        : dir{dir_}
        //, watcher{}
        , contents{}
        , renameCache{}
        , observer{std::move(observer)}
        , saveChange{saveChange}
    {
        if (scan)
            cache.fullscan();
        /*
        watcher = std::make_unique <filewatch::FileWatch <std::wstring>>
        (
            dir.wstring(),
            [watch](const std::wstring& path, const filewatch::Event change_type){watch(path, change_type);}
        );
        */
    }
//#####################################################################################################################
    DirectoryCache::DirectoryCache(Filesystem::path const& dir, CacheUpdateObserver observer, bool scan, bool saveChanges)
        : impl_{new DirectoryCache::Implementation
        (
            *this,
            dir,
            /*
            [this](std::string const& path, const filewatch::Event event){
                switch(event)
                {
                case filewatch::Event::added:
                    return onAdd(path);
                case filewatch::Event::removed:
                    return onRemove(path);
                case filewatch::Event::modified:
                    return onModify(path);
                case filewatch::Event::renamed_old:
                    return onRenamedOld(path);
                case filewatch::Event::renamed_new:
                    return onRenamedNew(path);
                default: return;
                }
            },
            */
            std::move(observer),
            scan,
            saveChanges
        )}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    DirectoryCache::~DirectoryCache() = default;
//---------------------------------------------------------------------------------------------------------------------
    void DirectoryCache::fullscan()
    {
        impl_->contents.clear();
        std::filesystem::directory_iterator iter{impl_->dir}, end;
        for (; iter != end; ++iter)
            impl_->contents.insert(iter->path().filename().string());
    }
//---------------------------------------------------------------------------------------------------------------------
    void DirectoryCache::onAdd(std::string const& path)
    {
        if (impl_->saveChange)
            impl_->contents.insert(path);
        impl_->observer.onAdd(*this, path);
    }
//---------------------------------------------------------------------------------------------------------------------
    void DirectoryCache::onRemove(std::string const& path)
    {
        if (impl_->saveChange)
            impl_->contents.erase(path);
        impl_->observer.onRemove(*this, path);
    }
//---------------------------------------------------------------------------------------------------------------------
    void DirectoryCache::onModify(std::string const& path)
    {
        impl_->observer.onModify(*this, path);
    }
//---------------------------------------------------------------------------------------------------------------------
    void DirectoryCache::onRenamedOld(std::string const& path)
    {
        if (impl_->saveChange)
            impl_->contents.erase(path);
        std::lock_guard <std::mutex> guard{impl_->renameLock};
        impl_->renameCache = path;
    }
//---------------------------------------------------------------------------------------------------------------------
    void DirectoryCache::onRenamedNew(std::string const& path)
    {
        if (impl_->saveChange)
            impl_->contents.insert(path);
        std::string old;
        // decouple locking from observer event.
        {
            std::lock_guard <std::mutex> guard{impl_->renameLock};
            old = impl_->renameCache;
        }
        impl_->observer.onRename(*this, old, path);
    }
//#####################################################################################################################
}
