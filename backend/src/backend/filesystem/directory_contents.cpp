#include <backend/filesystem/directory_contents.hpp>

#include <boost/container/stable_vector.hpp>

#include <iostream>

namespace Filesystem
{
    //#####################################################################################################################
    DirectoryContent::DirectoryContent(sfs::path const& createFrom)
        : root{createFrom}
        , files{}
        , directories{}
        , origin{}
        , flatDirectory{false}
    {}
    //---------------------------------------------------------------------------------------------------------------------
    void DirectoryContent::scan(bool recursive, int)
    {
        constexpr int dirStackShortening = 4096;
        namespace fs = sfs;
        boost::container::stable_vector<DirectoryContent*> dirStack;

        auto traverse = [&dirStack, this](DirectoryContent* recepticle) {
            for (auto const& i : fs::directory_iterator(recepticle->root))
            {
                if (fs::is_directory(i.status()))
                {
                    recepticle->directories.emplace_back(i.path());
                }
                if (fs::is_regular_file(i.status()))
                {
                    // fs::relative(i.path(), recepticle->root).string()
                    recepticle->files.push_back(i.path().filename().string());
                }
            }
            if (recepticle != this)
                recepticle->root = fs::relative(recepticle->root, root);
            for (auto& d : recepticle->directories)
                dirStack.emplace_back(&d);
        };

        if (recursive)
        {
            dirStack.emplace_back(this);
            for (std::size_t i = 0; i != dirStack.size(); ++i)
            {
                traverse(dirStack[i]);

                // decrease dir Stack size when possible.
                if (dirStack.size() > dirStackShortening && i >= (dirStackShortening / 4))
                {
                    dirStack.erase(std::begin(dirStack), std::begin(dirStack) + i);
                    i = 0;
                }
            }
        }
        else
        {
            traverse(this);
            flatDirectory = true;
            for (auto& d : directories)
                d.root = fs::relative(d.root, root);
            root = ".";
        }
    }
    //#####################################################################################################################
}
