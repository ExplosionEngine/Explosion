//
// Created by johnk on 2025/7/8.
//

#include <Common/File.h>
#include <Common/FileSystem.h>
#include <Test/Test.h>

TEST(FileTest, ReadWriteTextFileTest)
{
    static Common::Path file = "../Test/Generated/Common/ReadTextFileTest.txt";

    Common::FileUtils::WriteTextFile(file.Absolute().String(), "hello");
    const std::string content = Common::FileUtils::ReadTextFile(file.Absolute().String());
    ASSERT_EQ(content, "hello");
}
