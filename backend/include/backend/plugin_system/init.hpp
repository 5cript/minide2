#pragma once

#include <backend/filesystem/filesystem.hpp>

#include <memory>

namespace PluginSystem
{
    class GlobalInit
    {
    public:
        GlobalInit(sfs::path const& selfPath);
        ~GlobalInit();

    private:
        struct Implementation;
        std::unique_ptr<Implementation> impl_;
    };
}