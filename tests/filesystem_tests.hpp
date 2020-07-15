#pragma once

#include "../worker/filesystem/filesystem.hpp"

class NormalizeTests
    : public ::testing::Test
{
public:
    std::string normalizeToString(sfs::path const& p)
    {
        return Filesystem::linux_normalize(p).string();
    }
};

TEST_F(NormalizeTests, NormalizePaths)
{
    EXPECT_EQ(normalizeToString("//asdf/x/../a/b"), "//asdf/a/b");
    EXPECT_EQ(normalizeToString("C:/"), "C:/");
    EXPECT_EQ(normalizeToString("C:\\"), "C:/");
    EXPECT_EQ(normalizeToString("C:ab.txt"), "C:ab.txt");
    EXPECT_EQ(normalizeToString("C:////a"), "C:/a");
    EXPECT_EQ(normalizeToString("C:/../a"), "C:/a");
    EXPECT_EQ(normalizeToString("C:/../a/b"), "C:/a/b");
    EXPECT_EQ(normalizeToString("C:/x/../a/b"), "C:/a/b");
    EXPECT_EQ(normalizeToString(""), "");
}
