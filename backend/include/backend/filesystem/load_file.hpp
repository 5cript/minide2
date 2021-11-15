#pragma once

#include "filesystem.hpp"
#include <string>
#include <fstream>
#include <stdexcept>

namespace Filesystem
{
    inline std::string loadFile(sfs::path const& file) 
    {
        using namespace std::string_literals;
        std::ifstream reader{file, std::ios_base::binary};
        if (!reader.good())
            throw std::runtime_error("Cannot open and read file '"s + file.string() + "'.");

        reader.seekg(0, std::ios_base::end);
        std::string content(reader.tellg(), '\0');
        reader.seekg(0, std::ios_base::beg);
        reader.read(content.data(), content.size());
        return content;
    }
    
    inline std::string loadFile(std::ifstream& reader) 
    {
        reader.seekg(0, std::ios_base::end);
        std::string content(reader.tellg(), '\0');
        reader.seekg(0, std::ios_base::beg);
        reader.read(content.data(), content.size());
        return content;
    }
}