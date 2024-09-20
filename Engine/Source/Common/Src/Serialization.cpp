//
// Created by johnk on 2023/7/13.
//

#include <filesystem>

#include <Common/Serialization.h>

namespace Common {
    BinarySerializeStream::BinarySerializeStream() = default;

    BinarySerializeStream::~BinarySerializeStream() = default;

    BinaryDeserializeStream::BinaryDeserializeStream() = default;

    BinaryDeserializeStream::~BinaryDeserializeStream() = default;

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

    size_t BinaryFileSerializeStream::Loc()
    {
        return file.tellp();
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
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
    }

    BinaryFileDeserializeStream::~BinaryFileDeserializeStream()
    {
        Close();
    }

    void BinaryFileDeserializeStream::Read(void* data, const size_t size)
    {
        Assert(static_cast<size_t>(file.tellg()) + size <= fileSize);
        file.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
    }

    void BinaryFileDeserializeStream::Seek(int64_t offset)
    {
        file.seekg(offset, std::ios::cur);
    }

    size_t BinaryFileDeserializeStream::Loc()
    {
        return file.tellg();
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

    MemorySerializeStream::MemorySerializeStream(std::vector<uint8_t>& inBytes, const size_t pointerBegin)
        : pointer(pointerBegin)
        , bytes(inBytes)
    {
        Assert(pointer <= bytes.size());
    }

    MemorySerializeStream::~MemorySerializeStream() = default;

    void MemorySerializeStream::Write(const void* data, const size_t size)
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

    void MemorySerializeStream::Seek(int64_t offset)
    {
        pointer += offset;
    }

    size_t MemorySerializeStream::Loc()
    {
        return pointer;
    }

    MemoryDeserializeStream::MemoryDeserializeStream(const std::vector<uint8_t>& inBytes, const size_t pointerBegin)
        : pointer(pointerBegin)
        , bytes(inBytes)
    {
        Assert(pointer <= bytes.size());
    }

    MemoryDeserializeStream::~MemoryDeserializeStream() = default;

    void MemoryDeserializeStream::Read(void* data, const size_t size)
    {
        const auto newPointer = pointer + size;
        Assert(newPointer <= bytes.size());
        memcpy(data, bytes.data() + pointer, size);
        pointer = newPointer;
    }

    void MemoryDeserializeStream::Seek(int64_t offset)
    {
        pointer += offset;
    }

    size_t MemoryDeserializeStream::Loc()
    {
        return pointer;
    }
}
