//
// Created by Mevol on 2021/8/8.
//

#include <FileSystem/WriteStream.h>
#include <FileSystem/File.h>

namespace Explosion::FileSystem {
    WriteStream::WriteStream(File& file) : Stream<WriteStream>(file) {}

    WriteStream::~WriteStream() = default;

    WriteStream& WriteStream::operator()(uint32_t pos)
    {
        if (IsOpen()) {
            fileStream.seekp(pos);
            return *this;
        }
        throw std::runtime_error("ReadStream::operator()(): Stream Open Failed!");
    }

    void WriteStream::Open(const FileType& fileType)
    {
        if (fileType == FileType::TEXT) {
            openType = FileType::TEXT;
            fileStream.open(file.GetAbsolutePath().c_str(), std::ios::out);
            if (!fileStream.fail()) {
                failFlag  = false;
                openFlag = true;
            }
        }
        if (fileType == FileType::BINARY) {
            openType = FileType::BINARY;
            fileStream.open(file.GetAbsolutePath().c_str(), std::ios::out | std::ios::binary);
            if (!fileStream.fail()) {
                failFlag  = false;
                openFlag = true;
            }
        }
    }

    uint32_t WriteStream::SizeImpl()
    {
        if (IsOpen()) {
            return fileStream.tellp();
        }
        return 0;
    }
}
