#include "file_content.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>

using namespace std::string_literals;

namespace Streaming::Messages
{
//#####################################################################################################################
    std::string FileContent::toJson() const
    {
        auto j = json{
            {"type", "file_content"s},
            {"chunks", chunks},
            {"flag", flag},
            {"path", path},
        };

        if (line)
            j["line"] = line.value();
        if (linePos)
            j["linePos"] = linePos.value();

        j.dump();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string FileContent::load(std::string const& file, bool forceLoad, long long max, long long forceMax, long long chunkSize)
    {
        std::ifstream reader{file, std::ios_base::binary};
        if (!reader.good())
            return "cannot open file"s;

        reader.seekg(0, std::ios::end);
        if (!forceLoad && reader.tellg() > max)
            return "file exceeds maximum size for opening"s;

        if (reader.tellg() > forceMax)
            return "file exceeds maximum size for opening (even when forced to)"s;

        std::size_t amountLeft = reader.tellg();
        reader.seekg(0);
        while (amountLeft > 0)
        {
            auto cappedRead = std::min(static_cast <std::size_t> (chunkSize), amountLeft);

            std::string chunk;
            chunk.resize(cappedRead);

            reader.read(&chunk[0], cappedRead);
            amountLeft -= reader.gcount();

            chunks.push_back(chunk);
        }

        return "";
    }
//#####################################################################################################################
}
