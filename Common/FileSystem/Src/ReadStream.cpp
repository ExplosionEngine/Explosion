//
// Created by John Kindem on 2021/8/8.
//

#include <FileSystem/ReadStream.h>
#include <FileSystem/File.h>

namespace Explosion::FileSystem {
    ReadStream::ReadStream(File& file) : Stream<ReadStream>(file) {}

    ReadStream::~ReadStream() = default;

    ReadStream& ReadStream::operator()(uint32_t pos)
    {
        if (IsOpen()) {
            fileStream.seekg(pos);
            return *this;
        }
        throw std::runtime_error("ReadStream::operator()(): Stream Open Failed!");
    }

    void ReadStream::Open(const FileType& fileType)
    {
        if (fileType == FileType::TEXT)
        {
            openType = FileType::TEXT;
            fileStream.open(file.GetAbsolutePath().c_str(), std::ios::in);
            if (!fileStream.fail()) {
                failFlag = false;
                openFlag = true;
            }
        }
        if(fileType == FileType::BINARY)
        {
            openType = FileType::BINARY;
            fileStream.open(file.GetAbsolutePath().c_str(), std::ios::in | std::ios::binary);
            if (!fileStream.fail()) {
                failFlag = false;
                openFlag = true;
            }
        }
    }

    size_t ReadStream::SizeImpl()
    {
        if (IsOpen()) {
            return fileStream.tellg();
        }
        return 0;
    }
}
