//
// Created by Mevol on 2021-08-08.
//
#include <gtest/gtest.h>
#include <FileSystem/File.h>
#include <FileSystem/Stream.h>
#include <FileSystem/ReadStream.h>
#include <FileSystem/WriteStream.h>
#include <FileSystem/Directory.h>

using namespace Explosion::FileSystem;

TEST(FileSystemTest, StringReadWriteTest)
{
    File tmpFile("./TestFileSystem.txt");
    WriteStream wStream(tmpFile);
    std::string inputStr = "TestFileSystem";
    wStream.Open(FileType::TEXT);
    ASSERT_FALSE(wStream.Fail());
    wStream << inputStr;
    wStream.Close();

    ReadStream rStream(tmpFile);
    std::string outputStr = "";
    rStream.Open(FileType::TEXT);
    ASSERT_FALSE(rStream.Fail());
    rStream >> outputStr;
    rStream.Close();

    ASSERT_STREQ(inputStr.c_str(),outputStr.c_str());
}

TEST(FileSystemTest, NumReadWriteTest)
{
    File tmpFile("./TestFileSystem.txt");
    WriteStream wStream(tmpFile);
    int32_t inputNum = 14356712;
    wStream.Open(FileType::TEXT);
    ASSERT_FALSE(wStream.Fail());
    wStream << inputNum;
    wStream.Close();

    ReadStream rStream(tmpFile);
    int32_t outputNum = 0;
    rStream.Open(FileType::TEXT);
    ASSERT_FALSE(rStream.Fail());
    rStream >> outputNum;
    rStream.Close();

    ASSERT_EQ(inputNum,outputNum);
}