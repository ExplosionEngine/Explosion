//
// Created by johnk on 2025/7/8.
//

#include <Common/File.h>
#include <Common/FileSystem.h>
#include <Test/Test.h>

TEST(FileTest, ReadWriteTextFileTest)
{
    static Common::Path file = "../Test/Generated/Common/ReadTextFileTest.txt";

    ASSERT_TRUE(Common::FileUtils::WriteTextFile(file.Absolute().String(), "hello").IsOk());
    const auto readResult = Common::FileUtils::ReadTextFile(file.Absolute().String());
    ASSERT_TRUE(readResult.IsOk());
    ASSERT_EQ(readResult.Value(), "hello");
}

TEST(FileTest, ReadMissingTextFileTest)
{
    const auto readResult = Common::FileUtils::ReadTextFile("../Test/Generated/Common/DoesNotExist.txt");
    ASSERT_TRUE(readResult.IsErr());
    ASSERT_FALSE(readResult.Error().empty());
}

TEST(FileTest, ReadMissingJsonFileTest)
{
    const auto readResult = Common::FileUtils::ReadJsonFile("../Test/Generated/Common/DoesNotExist.json");
    ASSERT_TRUE(readResult.IsErr());
}
