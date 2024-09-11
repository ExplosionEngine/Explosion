//
// Created by johnk on 2023/7/13.
//

#include <filesystem>

#include <Common/Serialization.h>

namespace Common {
    SerializeStream::SerializeStream() = default;

    SerializeStream::~SerializeStream() = default;

    void SerializeStream::WriteTyped(const void* data, size_t size, uint32_t typeCrc)
    {
        Write(&typeCrc, sizeof(uint32_t));
        Write(&size, sizeof(size_t));
        Write(data, size);
    }

    DeserializeStream::DeserializeStream() = default;

    DeserializeStream::~DeserializeStream() = default;

    bool DeserializeStream::ReadTyped(void* data, size_t size, uint32_t typeCrc)
    {
        uint32_t tempCrc = 0;
        Read(&tempCrc, sizeof(uint32_t));

        size_t tempSize = 0;
        Read(&tempSize, sizeof(size_t));

        if (tempCrc != typeCrc || tempSize != size) {
            Seek(static_cast<int64_t>(tempSize));
            return false;
        }

        Read(data, size);
        return true;
    }

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

    void BinaryFileSerializeStream::Seek(int64_t offset)
    {
        file.seekp(offset, std::ios::cur);
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

    void BinaryFileDeserializeStream::Seek(int64_t offset)
    {
        file.seekg(offset, std::ios::cur);
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

    void ByteSerializeStream::Seek(int64_t offset)
    {
        pointer += offset;
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

    void ByteDeserializeStream::Seek(int64_t offset)
    {
        pointer += offset;
    }
}
