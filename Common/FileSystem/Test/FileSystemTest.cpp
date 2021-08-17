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

const std::string testCaseDir = "./FileSystemTestCase";

void remakeDir()
{
    fs::path path(testCaseDir);
    fs::remove_all(path);
    fs::create_directories(path);
}

void removeDir()
{
    fs::path path(testCaseDir);
    fs::remove_all(path);
}


TEST(FileSystemTest, StringReadWriteTest)
{
    remakeDir();
    File tmpFile(testCaseDir + "/" + "StringReadWriteTest.txt");
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
    removeDir();
}

TEST(FileSystemTest, NumberReadWriteTest)
{
    remakeDir();
    File tmpFile(testCaseDir + "/" + "NumberReadWriteTest.txt");
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
    removeDir();
}

TEST(FileSystemTest, FileInterfaceTest)
{
    remakeDir();
    std::string filename = "FileTestCase";
    std::string extension = ".md";
    std::string fullname = filename + extension;
    File tmpFile(testCaseDir + "/" + fullname);
    if (!tmpFile.IsExists()) {
        tmpFile.Make();
    }

    ASSERT_TRUE(tmpFile.IsExists());
    ASSERT_STREQ(tmpFile.GetFullName().c_str(),fullname.c_str());
    ASSERT_STREQ(tmpFile.GetExtension().c_str(),extension.c_str());
    ASSERT_STREQ(tmpFile.GetName().c_str(),filename.c_str());
    ASSERT_STREQ(tmpFile.GetRelativePath(tmpFile.GetParent()).c_str(),fullname.c_str());
    removeDir();
}

TEST(FileSystemTest, DirectoryInterfaceTest)
{
    remakeDir();
    std::vector<File> fileVec;
    std::vector<Directory> dirVec;
    for (int i = 0; i < 10; ++i) {
        std::string fileName = testCaseDir + "/testFile" + std::to_string(i);
        File tmpFile(fileName);
        tmpFile.Make();
        ASSERT_TRUE(tmpFile.IsExists());
        fileVec.push_back(tmpFile);
    }
    for (int i = 0; i < 10; ++i) {
        std::string dirName = testCaseDir + "/testDir" + std::to_string(i);
        Directory tmpDir(dirName);
        tmpDir.Make();
        ASSERT_TRUE(tmpDir.IsExists());
        dirVec.push_back(tmpDir);
    }

    Directory testDir(testCaseDir);
    std::vector<File> fileListResult = testDir.ListFile();
    std::vector<Directory> dirListResult = testDir.ListDir();
    for (int i = 0; i < fileVec.size(); i++) {
        bool sameFlag = false;
        for (int j = 0; j < fileListResult.size(); j++) {
            if (fileVec.at(i).GetAbsolutePath() == fileListResult[j].GetAbsolutePath()) {
                sameFlag = true;
                break;
            }
        }
        EXPECT_TRUE(sameFlag) << fileVec.at(i).GetAbsolutePath() << " not exist in vector 'fileListResult'!" ;
    }
    for (int i = 0; i < dirVec.size(); i++) {
        bool sameFlag = false;
        for (int j = 0; j < dirListResult.size(); j++) {
            if (dirVec.at(i).GetAbsolutePath() == dirListResult[j].GetAbsolutePath()) {
                sameFlag = true;
                break;
            }
        }
        EXPECT_TRUE(sameFlag) << dirVec.at(i).GetAbsolutePath() << " not exist in vector 'fileListResult'!" ;
    }
    removeDir();
}