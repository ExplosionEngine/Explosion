//
// Created by johnk on 2023/7/13.
//

#include <filesystem>

#include <Common/Serialization.h>

namespace Common {
    SerializeStream::SerializeStream() = default;

    SerializeStream::~SerializeStream() = default;

    DeserializeStream::DeserializeStream() = default;

    DeserializeStream::~DeserializeStream() = default;

    BinaryFileSerializeStream::BinaryFileSerializeStream(const std::string& inFileName)
    {
        const auto parent_path = std::filesystem::path(inFileName).parent_path();
        if (!std::filesystem::exists(parent_path)) {
            std::filesystem::create_directories(parent_path);
        }
        file = std::ofstream(inFileName, std::ios::binary);
    }

    BinaryFileSerializeStream::~BinaryFileSerializeStream()
    {
        try {
            file.close();
        } catch (const std::exception& e) {
            QuickFail();
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
            QuickFail();
        }
    }

    void BinaryFileDeserializeStream::Read(void* data, size_t size)
    {
        file.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
    }

    ByteSerializeStream::ByteSerializeStream(std::vector<uint8_t>& inBytes, size_t pointerBegin)
        : pointer(pointerBegin)
        , bytes(inBytes)
    {
        Assert(pointer <= bytes.size());
    }

    ByteSerializeStream::~ByteSerializeStream() = default;

    void ByteSerializeStream::Write(const void* data, size_t size)
    {
        auto newPointer = pointer + size;
        if (newPointer > bytes.size()) {
            if (newPointer > bytes.capacity()) {
                bytes.reserve(std::ceil(static_cast<float>(newPointer) * 1.5f));
            }
            bytes.resize(newPointer);
        }
        memcpy(bytes.data() + pointer, data, size);
        pointer = newPointer;
    }

    ByteDeserializeStream::ByteDeserializeStream(const std::vector<uint8_t>& inBytes, size_t pointerBegin)
        : pointer(pointerBegin)
        , bytes(inBytes)
    {
        Assert(pointer <= bytes.size());
    }

    ByteDeserializeStream::~ByteDeserializeStream() = default;

    void ByteDeserializeStream::Read(void* data, size_t size)
    {
        const auto newPointer = pointer + size;
        Assert(newPointer <= bytes.size());
        memcpy(data, bytes.data() + pointer, size);
        pointer = newPointer;
    }
}
