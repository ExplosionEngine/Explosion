//
// Created by johnk on 2023/7/13.
//

#include <Common/Serialization.h>

namespace Common {
    SerializeStream::SerializeStream() = default;

    SerializeStream::~SerializeStream() = default;

    DeserializeStream::DeserializeStream() = default;

    DeserializeStream::~DeserializeStream() = default;

    BinaryFileSerializeStream::BinaryFileSerializeStream(const std::string& inFileName)
        : file(inFileName, std::ios::binary)
    {
    }

    BinaryFileSerializeStream::~BinaryFileSerializeStream()
    {
        try {
            file.close();
        } catch (const std::exception& e) {
            Assert(false);
        }
    }

    void BinaryFileSerializeStream::Write(const void* data, size_t size)
    {
        file.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
    }

    BinaryFileDeserializeStream::BinaryFileDeserializeStream(const std::string& inFileName)
        : file(inFileName, std::ios::binary)
    {
    }

    BinaryFileDeserializeStream::~BinaryFileDeserializeStream()
    {
        try {
            file.close();
        } catch (const std::exception& e) {
            Assert(false);
        }
    }

    void BinaryFileDeserializeStream::Read(void* data, size_t size)
    {
        file.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
    }
}
