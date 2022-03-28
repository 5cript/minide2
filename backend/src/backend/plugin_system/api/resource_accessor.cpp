#include <backend/plugin_system/api/resource_accessor.hpp>

#include <backend/filesystem/jail.hpp>
#include <backend/filesystem/load_file.hpp>

#include <boost/beast/core/detail/base64.hpp>
#include <boost/gil.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/extension/io/png.hpp>

#include <fstream>
#include <stdexcept>
#include <string>

using namespace std::string_literals;

namespace Backend::PluginSystem::PluginApi
{
    //#####################################################################################################################
    ResourceAccessor::ResourceAccessor(std::filesystem::path const& resourceDirectory)
        : resourceDirectory_{resourceDirectory}
    {}
    //---------------------------------------------------------------------------------------------------------------------
    std::string ResourceAccessor::loadPng(std::string const& fileName)
    {
        Filesystem::Jail jail{resourceDirectory_};
        if (auto maybePath = jail.relativeToRoot(fileName); maybePath)
        {
            std::ifstream reader{*maybePath, std::ios_base::binary};
            if (!reader.good())
                throw std::runtime_error("Could not open file at '"s + maybePath->string() + "'.");

            // verify image only:
            namespace bg = boost::gil;
            bg::rgba8_image_t image;
            bg::read_and_convert_image(reader, image, bg::image_read_settings<bg::png_tag>{});

            reader.seekg(0);
            auto content = Filesystem::loadFile(reader);
            std::string encoded(boost::beast::detail::base64::encoded_size(content.size()), '\0');
            boost::beast::detail::base64::encode(encoded.data(), content.c_str(), content.size());
            return encoded;
        }
        else
            throw std::runtime_error("Path was rejected. Its probably not within the resource directory.");
    }
    //#####################################################################################################################
}