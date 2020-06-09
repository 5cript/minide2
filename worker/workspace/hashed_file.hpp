#pragma once

#include <fstream>
#include <string>

class HashedFile
{
public:
    HashedFile();
    ~HashedFile();

    /**
     *  Opens a file, but as a temporary first instead.
     */
    void open(std::string const& fileName, std::string const& hash);

    /**
     *  Could open/create file?
     */
    bool good() const;

    /**
     *  Write to file.
     */
    void write(char const* data, std::size_t amount);

    /**
     *  Test file hash and move it.
     */
    bool testAndMove();

private:
    std::string hash_;
    std::string originalName_;
    std::ofstream writer_;
};
