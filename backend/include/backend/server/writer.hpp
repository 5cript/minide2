#pragma once

#include <backend/filesystem/filesystem.hpp>

#include <memory>
#include <string>
#include <fstream>

namespace Backend::Server
{
    class FrontendUserSession;

    class Writer
    {
      public:
        virtual ~Writer() = default;
        virtual void write() = 0;
    };

    class FileWriter
        : public Writer
        , public std::enable_shared_from_this<FileWriter>
    {
      public:
        constexpr static unsigned bufferSize = 4096;

        FileWriter(int ref, sfs::path const& file, std::weak_ptr<FrontendUserSession> session);
        ~FileWriter() = default;
        void write() override;
        bool good() const;

      private:
        int ref_;
        std::ifstream reader_;
        std::string buffer_;
        std::weak_ptr<FrontendUserSession> session_;
    };
}