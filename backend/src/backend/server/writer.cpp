#include <backend/server/writer.hpp>
#include <backend/server/frontend_user_session.hpp>

//#####################################################################################################################
FileWriter::FileWriter(int ref, sfs::path const& file, std::weak_ptr<FrontendUserSession> session)
    : ref_{ref}
    , reader_{file, std::ios_base::binary}
    , buffer_(bufferSize, '\0')
    , session_{session}
{}
//---------------------------------------------------------------------------------------------------------------------
void FileWriter::write()
{
    if (auto sess = session_.lock(); sess)
    {
        reader_.read(buffer_.data(), buffer_.size());
        const auto readCount = static_cast<std::size_t>(reader_.gcount());
        sess->writeBinary(ref_, buffer_, readCount, [self = shared_from_this(), readCount](auto, auto) {
            if (readCount == bufferSize)
                self->write();
            else if (auto sess = self->session_.lock(); sess)
            {
                sess->writeBinary(self->ref_, "", 0, [](auto, auto) {});
            }
        });
    }
}
//---------------------------------------------------------------------------------------------------------------------
bool FileWriter::good() const
{
    return reader_.good();
}
//#####################################################################################################################