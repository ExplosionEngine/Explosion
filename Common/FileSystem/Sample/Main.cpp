//
// Created by Mevol on 2021-08-08.
//
#include <iostream>
#include <FileSystem/File.h>
#include <FileSystem/Stream.h>
#include <FileSystem/ReadStream.h>
#include <FileSystem/WriteStream.h>
#include <FileSystem/Directory.h>

using namespace Explosion::FileSystem;
int main()
{
    File tmpFile("./FSSamplePath/test.cpp");

    if (tmpFile.IsExists()) {
        std::cout << "file exist!" << std::endl;
    } else {
        std::cout << "file not exist!now create file" << std::endl;
        tmpFile.Make();
    }

    std::cout << "relative path: " << tmpFile.GetRelativePath("./") << std::endl;
    std::cout << "absolute path: " << tmpFile.GetAbsolutePath() << std::endl;
    std::cout << "extension: " << tmpFile.GetExtension() << std::endl;
    std::cout << "fullname: " << tmpFile.GetFullName() << std::endl;
    std::cout << "filename: " << tmpFile.GetName() << std::endl;
    std::cout << "parent: " << tmpFile.GetParent() << std::endl;
    std::cout << "isDir: " << tmpFile.IsDirectory() << std::endl;
    std::cout << "isFile: " << tmpFile.IsFile() << std::endl;

    WriteStream wStream(tmpFile);
    wStream.Open(FileType::TEXT);
    if (wStream.Fail()) {
        return 0;
    }
    wStream << "Explosion!";
    wStream << "\n";
    wStream << 1234567890;
    wStream.Close();
    ReadStream rStream(tmpFile);
    rStream.Open(FileType::TEXT);
    if (rStream.Fail()) {
        return 0;
    }
    std::string str;
    int num;
    rStream >> str;
    rStream >> num;
    std::cout << "str: " << str << std::endl << "num: " << num << std::endl;
    rStream.Close();

    tmpFile.Rename("./FSSamplePath/test2.cpp");
    std::cout << "after rename file exist?" << tmpFile.IsExists() << std::endl;

    Directory tmpDir("./");
    std::vector<File> fileList(std::move(tmpDir.ListFile()));
    std::vector<Directory> dirList(std::move(tmpDir.ListDir()));
    for (auto iter : fileList) {
        std::cout << "File : " << iter.GetAbsolutePath() << std::endl;
    }
    for (auto iter : dirList) {
        std::cout << "Dir : " << iter.GetAbsolutePath() << std::endl;
    }

    return 0;
}