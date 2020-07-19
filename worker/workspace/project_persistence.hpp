#pragma once

#include "../filesystem/filesystem.hpp"
#include "../json.hpp"

#include <string>

class ProjectPersistence
{
public:
    ProjectPersistence(sfs::path const& root);

    bool save() const;
    bool load();
    std::string raw();

    /**
     *  Merges these settings into the persistence.
     */
    void inject(json const& j);

public:
    bool splitSourceAndInclude;
    bool ignoreSeemingSplit;

private:
    void transferExistingItems(json const& j);

private:
    sfs::path root_;
    std::string raw_;
};
