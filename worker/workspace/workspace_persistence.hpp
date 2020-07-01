#pragma once

#include "../filesystem/filesystem.hpp"

#include <string>

class WorkspacePersistence
{
public:
    WorkspacePersistence(sfs::path const& root);

    bool save();
    bool load();
    std::string raw() const;

public:
    std::string lastActiveProject;

private:
    sfs::path root_;
    std::string raw_;
};
