#include "hashed_file.hpp"

#include "../filesystem/filesystem.hpp"

#include <hash-library/sha256.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <cerrno>

using namespace Filesystem;

//#####################################################################################################################
HashedFile::HashedFile()
    : hash_{}
    , originalName_{}
    , writer_{}
{
}
//---------------------------------------------------------------------------------------------------------------------
HashedFile::~HashedFile()
{
    try
    {
        writer_.close();
        if (sfs::exists(originalName_) && sfs::exists(originalName_ + "_minide2_temp"))
            remove((originalName_ + "_minide2_temp").c_str());
    }
    catch(...)
    {
        // eat it
    }
}
//---------------------------------------------------------------------------------------------------------------------
void HashedFile::open(std::string const& fileName, std::string const& hash)
{
    originalName_ = fileName;
    hash_ = hash;

    std::transform(std::begin(hash_), std::end(hash_), std::begin(hash_), [](char c){return std::tolower(c);});

    writer_.open(fileName + "_minide2_temp", std::ios_base::binary);
}
//---------------------------------------------------------------------------------------------------------------------
bool HashedFile::good() const
{
    return writer_.good();
}
//---------------------------------------------------------------------------------------------------------------------
void HashedFile::write(char const* data, std::size_t amount)
{
    writer_.write(data, amount);
}
//---------------------------------------------------------------------------------------------------------------------
bool HashedFile::testAndMove()
{
    writer_.close();

    std::string file = originalName_ + "_minide2_temp";
    SHA256 shaStream;
    {
        std::ifstream reader{file, std::ios_base::binary};
        std::vector <char> buffer(1024);
        do {
            reader.read(&buffer[0], buffer.size());
            if (reader.gcount() > 0)
                shaStream.add(&buffer[0], reader.gcount());
        } while (reader.gcount() > 0);
    }

    if (hash_ != shaStream.getHash())
    {
        return false;
    }
    else
    {
        if (sfs::exists(originalName_))
            remove(originalName_.c_str());
        auto res = std::rename((originalName_ + "_minide2_temp").c_str(), originalName_.c_str());
        return res == 0;
    }

}
//#####################################################################################################################
