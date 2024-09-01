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

    EmptySerializeStream::EmptySerializeStream() = default;

    EmptySerializeStream::~EmptySerializeStream() = default;

    void EmptySerializeStream::Write(const void* data, size_t size) {}

    EmptyDeserializeStream::EmptyDeserializeStream() = default;

    EmptyDeserializeStream::~EmptyDeserializeStream() = default;

    void EmptyDeserializeStream::Read(void* data, size_t size) {}

    BinaryFileSerializeStream::BinaryFileSerializeStream(const std::string& inFileName)
    {
        if (const auto parent_path = std::filesystem::path(inFileName).parent_path();
            !std::filesystem::exists(parent_path)) {
            std::filesystem::create_directories(parent_path);
        }
        file = std::ofstream(inFileName, std::ios::binary);
    }

    BinaryFileSerializeStream::~BinaryFileSerializeStream()
    {
        Close();
    }

    void BinaryFileSerializeStream::Write(const void* data, const size_t size)
    {
        file.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
    }

    void BinaryFileSerializeStream::Close()
    {
        if (!file.is_open()) {
            return;
        }
        try {
            file.close();
        } catch (const std::exception&) {
            QuickFail();
        }
    }

    BinaryFileDeserializeStream::BinaryFileDeserializeStream(const std::string& inFileName)
        : file(inFileName, std::ios::binary)
    {
    }

    BinaryFileDeserializeStream::~BinaryFileDeserializeStream()
    {
        Close();
    }

    void BinaryFileDeserializeStream::Read(void* data, const size_t size)
    {
        file.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
    }

    void BinaryFileDeserializeStream::Close()
    {
        if (!file.is_open()) {
            return;
        }
        try {
            file.close();
        } catch (const std::exception&) {
            QuickFail();
        }
    }

    ByteSerializeStream::ByteSerializeStream(std::vector<uint8_t>& inBytes, const size_t pointerBegin)
        : pointer(pointerBegin)
        , bytes(inBytes)
    {
        Assert(pointer <= bytes.size());
    }

    ByteSerializeStream::~ByteSerializeStream() = default;

    void ByteSerializeStream::Write(const void* data, const size_t size)
    {
        const auto newPointer = pointer + size;
        if (newPointer > bytes.size()) {
            if (newPointer > bytes.capacity()) {
                bytes.reserve(std::ceil(static_cast<float>(newPointer) * 1.5f));
            }
            bytes.resize(newPointer);
        }
        memcpy(bytes.data() + pointer, data, size);
        pointer = newPointer;
    }

    ByteDeserializeStream::ByteDeserializeStream(const std::vector<uint8_t>& inBytes, const size_t pointerBegin)
        : pointer(pointerBegin)
        , bytes(inBytes)
    {
        Assert(pointer <= bytes.size());
    }

    ByteDeserializeStream::~ByteDeserializeStream() = default;

    void ByteDeserializeStream::Read(void* data, const size_t size)
    {
        const auto newPointer = pointer + size;
        Assert(newPointer <= bytes.size());
        memcpy(data, bytes.data() + pointer, size);
        pointer = newPointer;
    }
}
