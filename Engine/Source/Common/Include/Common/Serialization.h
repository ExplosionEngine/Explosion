//
// Created by johnk on 2023/7/13.
//

#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <optional>
#include <array>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <variant>

#include <rapidjson/document.h>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/Hash.h>
#include <Common/String.h>
#include <Common/FileSystem.h>
#include <Common/File.h>

namespace Common {
    class BinarySerializeStream {
    public:
        NonCopyable(BinarySerializeStream)
        virtual ~BinarySerializeStream();

        template <CppArithmetic T> void Write(const T& value);
        virtual void Seek(int64_t offset) = 0;
        virtual size_t Loc() = 0;
        virtual std::endian Endian() = 0;

    protected:
        BinarySerializeStream();

        virtual void WriteInternal(const void* data, size_t size) = 0;
    };

    class BinaryDeserializeStream {
    public:
        NonCopyable(BinaryDeserializeStream);
        virtual ~BinaryDeserializeStream();

        template <CppArithmetic T> void Read(T& value);
        virtual void Seek(int64_t offset) = 0;
        virtual size_t Loc() = 0;
        virtual std::endian Endian() = 0;

    protected:
        BinaryDeserializeStream();

        virtual void ReadInternal(void* data, size_t size) = 0;
    };

    template <std::endian E = std::endian::little>
    class BinaryFileSerializeStream final : public BinarySerializeStream {
    public:
        NonCopyable(BinaryFileSerializeStream)
        explicit BinaryFileSerializeStream(const std::string& inFileName);
        ~BinaryFileSerializeStream() override;

        void Seek(int64_t offset) override;
        size_t Loc() override;
        std::endian Endian() override;
        void Close();

    protected:
        void WriteInternal(const void* data, size_t size) override;

    private:
        std::ofstream file;
    };

    template <std::endian E = std::endian::little>
    class BinaryFileDeserializeStream final : public BinaryDeserializeStream {
    public:
        NonCopyable(BinaryFileDeserializeStream)
        explicit BinaryFileDeserializeStream(const std::string& inFileName);
        ~BinaryFileDeserializeStream() override;

        void Seek(int64_t offset) override;
        size_t Loc() override;
        std::endian Endian() override;
        void Close();

    protected:
        void ReadInternal(void* data, size_t size) override;

    private:
        std::ifstream file;
        size_t fileSize;
    };

    template <std::endian E = std::endian::little>
    class MemorySerializeStream final : public BinarySerializeStream {
    public:
        NonCopyable(MemorySerializeStream)
        explicit MemorySerializeStream(std::vector<uint8_t>& inBytes, size_t pointerBegin = 0);
        ~MemorySerializeStream() override;

        void Seek(int64_t offset) override;
        size_t Loc() override;
        std::endian Endian() override;

    protected:
        void WriteInternal(const void* data, size_t size) override;

    private:
        size_t pointer;
        std::vector<uint8_t>& bytes;
    };

    template <std::endian E = std::endian::little>
    class MemoryDeserializeStream final : public BinaryDeserializeStream {
    public:
        NonCopyable(MemoryDeserializeStream)
        explicit MemoryDeserializeStream(const std::vector<uint8_t>& inBytes, size_t pointerBegin = 0);
        ~MemoryDeserializeStream() override;

        void Seek(int64_t offset) override;
        std::endian Endian() override;
        size_t Loc() override;

    protected:
        void ReadInternal(void* data, size_t size) override;

    private:
        size_t pointer;
        const std::vector<uint8_t>& bytes;
    };

    template <typename T> struct Serializer {};
    template <typename T> concept Serializable = requires(T inValue, BinarySerializeStream& serializeStream, BinaryDeserializeStream& deserializeStream)
    {
        { Serializer<T>::typeId } -> std::convertible_to<uint32_t>;
        { Serializer<T>::Serialize(serializeStream, inValue) } -> std::convertible_to<size_t>;
        { Serializer<T>::Deserialize(deserializeStream, inValue) } -> std::convertible_to<size_t>;
    };

    template <Serializable T> struct FieldSerializer;

    template <typename T> size_t Serialize(BinarySerializeStream& inStream, const T& inValue);
    template <typename T> std::pair<bool, size_t> Deserialize(BinaryDeserializeStream& inStream, T& outValue);
    template <typename T> void SerializeToFile(const std::string& inFile, const T& inValue);
    template <typename T> bool DeserializeFromFile(const std::string& inFile, T& outValue);

    template <typename T> struct JsonSerializer {};
    template <typename T> concept JsonSerializable = requires(
        const T& inValue, T& outValue,
        const rapidjson::Value& inJsonValue, rapidjson::Value& outJsonValue,
        rapidjson::Document::AllocatorType& inAllocator)
    {
        JsonSerializer<T>::JsonSerialize(outJsonValue, inAllocator, inValue);
        JsonSerializer<T>::JsonDeserialize(inJsonValue, outValue);
    };

    template <typename T> void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const T& inValue);
    template <typename T> void JsonDeserialize(const rapidjson::Value& inJsonValue, T& outValue);
    template <typename T> void JsonSerializeToFile(const std::string& inFile, const T& inValue, bool inPretty = true);
    template <typename T> void JsonDeserializeFromFile(const std::string& inFile, T& outValue);
}

#define IMPL_BASIC_TYPE_SERIALIZER(typeName) \
    template <> \
    struct Serializer<typeName> { \
        static constexpr size_t typeId = HashUtils::StrCrc32(#typeName); \
        \
        static size_t Serialize(BinarySerializeStream& stream, const typeName& value) \
        { \
            stream.Write<typeName>(value); \
            return sizeof(typeName); \
        } \
        \
        static size_t Deserialize(BinaryDeserializeStream& stream, typeName& value) \
        { \
            stream.Read<typeName>(value); \
            return sizeof(typeName); \
        } \
    }; \

namespace Common::Internal {
    inline std::vector<uint8_t> SwapEndian(const void* data, const size_t size)
    {
        std::vector<uint8_t> result;
        result.resize(size);

        const auto* bytes = static_cast<const uint8_t*>(data);
        for (auto i = 0; i < size; i++) {
            result[i] = bytes[size - 1 - i];
        }
        return result;
    }

    inline void SwapEndianInplace(void* data, size_t size)
    {
        auto* bytes = static_cast<uint8_t*>(data);
        for (auto i = 0; i < size / 2; i++) {
            std::swap(bytes[i], bytes[size - 1 - i]);
        }
    }
}

namespace Common {
    template <CppArithmetic T>
    void BinarySerializeStream::Write(const T& value)
    {
        if (std::endian::native == Endian()) {
            WriteInternal(&value, sizeof(T));
        } else {
            const auto swapped = Internal::SwapEndian(&value, sizeof(T));
            WriteInternal(reinterpret_cast<const char*>(swapped.data()), static_cast<std::streamsize>(swapped.size()));
        }
    }

    template <CppArithmetic T>
    void BinaryDeserializeStream::Read(T& value)
    {
        ReadInternal(&value, sizeof(T));
        if (std::endian::native != Endian()) {
            Internal::SwapEndianInplace(&value, sizeof(T));
        }
    }

    template <std::endian E>
    BinaryFileSerializeStream<E>::BinaryFileSerializeStream(const std::string& inFileName)
    {
        if (const auto parentPath = Common::Path(inFileName).Parent();
            !parentPath.Exists()) {
            parentPath.MakeDir();
        }
        file = std::ofstream(inFileName, std::ios::binary);
    }

    template <std::endian E>
    BinaryFileSerializeStream<E>::~BinaryFileSerializeStream()
    {
        Close();
    }

    template <std::endian E>
    void BinaryFileSerializeStream<E>::WriteInternal(const void* data, const size_t size)
    {
        file.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
    }

    template <std::endian E>
    void BinaryFileSerializeStream<E>::Seek(int64_t offset)
    {
        file.seekp(offset, std::ios::cur);
    }

    template <std::endian E>
    size_t BinaryFileSerializeStream<E>::Loc()
    {
        return file.tellp();
    }

    template <std::endian E>
    std::endian BinaryFileSerializeStream<E>::Endian()
    {
        return E;
    }

    template <std::endian E>
    void BinaryFileSerializeStream<E>::Close()
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

    template <std::endian E>
    BinaryFileDeserializeStream<E>::BinaryFileDeserializeStream(const std::string& inFileName)
        : file(inFileName, std::ios::binary)
    {
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
    }

    template <std::endian E>
    BinaryFileDeserializeStream<E>::~BinaryFileDeserializeStream()
    {
        Close();
    }

    template <std::endian E>
    void BinaryFileDeserializeStream<E>::ReadInternal(void* data, const size_t size)
    {
        Assert(static_cast<size_t>(file.tellg()) + size <= fileSize);
        file.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
    }

    template <std::endian E>
    void BinaryFileDeserializeStream<E>::Seek(int64_t offset)
    {
        file.seekg(offset, std::ios::cur);
    }

    template <std::endian E>
    size_t BinaryFileDeserializeStream<E>::Loc()
    {
        return file.tellg();
    }

    template <std::endian E>
    std::endian BinaryFileDeserializeStream<E>::Endian()
    {
        return E;
    }

    template <std::endian E>
    void BinaryFileDeserializeStream<E>::Close()
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

    template <std::endian E>
    MemorySerializeStream<E>::MemorySerializeStream(std::vector<uint8_t>& inBytes, const size_t pointerBegin)
        : pointer(pointerBegin)
        , bytes(inBytes)
    {
        Assert(pointer <= bytes.size());
    }

    template <std::endian E>
    MemorySerializeStream<E>::~MemorySerializeStream() = default;

    template <std::endian E>
    void MemorySerializeStream<E>::WriteInternal(const void* data, const size_t size)
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

    template <std::endian E>
    void MemorySerializeStream<E>::Seek(int64_t offset)
    {
        pointer += offset;
    }

    template <std::endian E>
    size_t MemorySerializeStream<E>::Loc()
    {
        return pointer;
    }

    template <std::endian E>
    std::endian MemorySerializeStream<E>::Endian()
    {
        return E;
    }

    template <std::endian E>
    MemoryDeserializeStream<E>::MemoryDeserializeStream(const std::vector<uint8_t>& inBytes, const size_t pointerBegin)
        : pointer(pointerBegin)
        , bytes(inBytes)
    {
        Assert(pointer <= bytes.size());
    }

    template <std::endian E>
    MemoryDeserializeStream<E>::~MemoryDeserializeStream() = default;

    template <std::endian E>
    void MemoryDeserializeStream<E>::ReadInternal(void* data, const size_t size)
    {
        const auto newPointer = pointer + size;
        Assert(newPointer <= bytes.size());
        memcpy(data, bytes.data() + pointer, size);
        pointer = newPointer;
    }

    template <std::endian E>
    void MemoryDeserializeStream<E>::Seek(int64_t offset)
    {
        pointer += offset;
    }

    template <std::endian E>
    size_t MemoryDeserializeStream<E>::Loc()
    {
        return pointer;
    }

    template <std::endian E>
    std::endian MemoryDeserializeStream<E>::Endian()
    {
        return E;
    }

    template <typename T>
    size_t Serialize(BinarySerializeStream& inStream, const T& inValue)
    {
        if constexpr (Serializable<T>) {
            return FieldSerializer<T>::Serialize(inStream, inValue);
        } else {
            QuickFailWithReason("your type is not support serialization");
            return 0;
        }
    }

    template <typename T>
    std::pair<bool, size_t> Deserialize(BinaryDeserializeStream& inStream, T& outValue)
    {
        if constexpr (Serializable<T>) {
            return FieldSerializer<T>::Deserialize(inStream, outValue);
        } else {
            QuickFailWithReason("your type is not support serialization");
            return { false, 0 };
        }
    }

    template <typename T>
    void SerializeToFile(const std::string& inFile, const T& inValue)
    {
        BinaryFileSerializeStream stream(inFile);
        Serialize<T>(stream, inValue);
    }

    template <typename T>
    bool DeserializeFromFile(const std::string& inFile, T& outValue)
    {
        BinaryFileDeserializeStream stream(inFile);
        const auto result = Deserialize(stream, outValue);
        return result.first;
    }

    template <typename T>
    void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const T& inValue)
    {
        if constexpr (JsonSerializable<T>) {
            return JsonSerializer<T>::JsonSerialize(outJsonValue, inAllocator, inValue);
        } else {
            QuickFailWithReason("your type is not support json serialization");
        }
    }

    template <typename T>
    void JsonDeserialize(const rapidjson::Value& inJsonValue, T& outValue)
    {
        if constexpr (JsonSerializable<T>) {
            JsonSerializer<T>::JsonDeserialize(inJsonValue, outValue);
        } else {
            QuickFailWithReason("your type is not support json serialization");
        }
    }

    template <typename T> void JsonSerializeToFile(const std::string& inFile, const T& inValue, bool inPretty)
    {
        rapidjson::Document document;
        JsonSerialize<T>(document, document.GetAllocator(), inValue);
        FileUtils::WriteJsonFile(inFile, document, inPretty);
    }

    template <typename T> void JsonDeserializeFromFile(const std::string& inFile, T& outValue)
    {
        const rapidjson::Document document = FileUtils::ReadJsonFile(inFile);
        JsonDeserialize<T>(document, outValue);
    }

    template <Serializable T>
    struct FieldSerializer {
        struct Header {
            size_t typeId;
            size_t contentSize;

            void Serialize(BinarySerializeStream& stream) const
            {
                stream.Write<uint64_t>(static_cast<uint64_t>(typeId));
                stream.Write<uint64_t>(static_cast<uint64_t>(contentSize));
            }

            void Deserialize(BinaryDeserializeStream& stream)
            {
                uint64_t tempTypeId;
                stream.Read<uint64_t>(tempTypeId);
                typeId = static_cast<size_t>(tempTypeId);

                uint64_t tempContentSize;
                stream.Read<uint64_t>(tempContentSize);
                contentSize = static_cast<size_t>(tempContentSize);
            }
        };

        static size_t Serialize(BinarySerializeStream& stream, const T& value)
        {
            Header header;
            header.typeId = Serializer<T>::typeId;

            stream.Seek(sizeof(Header));
            header.contentSize = Serializer<T>::Serialize(stream, value);
            stream.Seek(-static_cast<int64_t>(sizeof(Header)) - static_cast<int64_t>(header.contentSize));
            header.Serialize(stream);
            stream.Seek(header.contentSize);
            return sizeof(Header) + header.contentSize;
        }

        static std::pair<bool, size_t> Deserialize(BinaryDeserializeStream& stream, T& value)
        {
            Header header {};
            header.Deserialize(stream);

            if (header.typeId != Serializer<T>::typeId) {
                stream.Seek(header.contentSize);
                return { false, sizeof(Header) };
            }

            size_t deserializedSize = Serializer<T>::Deserialize(stream, value);
            if (deserializedSize != header.contentSize) {
                stream.Seek(header.contentSize - deserializedSize);
                return { false, sizeof(Header) + deserializedSize };
            }
            return { true, sizeof(Header) + header.contentSize };
        }
    };

    IMPL_BASIC_TYPE_SERIALIZER(bool)
    IMPL_BASIC_TYPE_SERIALIZER(int8_t)
    IMPL_BASIC_TYPE_SERIALIZER(uint8_t)
    IMPL_BASIC_TYPE_SERIALIZER(int16_t)
    IMPL_BASIC_TYPE_SERIALIZER(uint16_t)
    IMPL_BASIC_TYPE_SERIALIZER(int32_t)
    IMPL_BASIC_TYPE_SERIALIZER(uint32_t)
    IMPL_BASIC_TYPE_SERIALIZER(int64_t)
    IMPL_BASIC_TYPE_SERIALIZER(uint64_t)
    IMPL_BASIC_TYPE_SERIALIZER(float)
    IMPL_BASIC_TYPE_SERIALIZER(double)

    template <>
    struct Serializer<std::string> {
        static constexpr size_t typeId = HashUtils::StrCrc32("std::string");

        static size_t Serialize(BinarySerializeStream& stream, const std::string& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            const auto* data = reinterpret_cast<const uint8_t*>(value.data());
            for (auto i = 0; i < size; i++) {
                stream.Write<uint8_t>(data[i]);
            }

            serialized += size;
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::string& value)
        {
            size_t deserialized = 0;

            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            value.resize(size);
            auto* data = reinterpret_cast<uint8_t*>(value.data());
            for (auto i = 0; i < size; i++) {
                stream.Read<uint8_t>(data[i]);
            }

            deserialized += size;
            return deserialized;
        }
    };

    template <>
    struct Serializer<std::wstring> {
        static constexpr size_t typeId = HashUtils::StrCrc32("std::wstring");
        // windows: 16, macOS: 32
        static_assert(sizeof(std::wstring::value_type) <= sizeof(uint32_t));

        static size_t Serialize(BinarySerializeStream& stream, const std::wstring& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            const auto* data = static_cast<const std::wstring::value_type*>(value.data());
            for (auto i = 0; i < size; i++) {
                stream.Write<uint32_t>(static_cast<uint32_t>(data[i]));
            }

            serialized += size * sizeof(uint32_t);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::wstring& value)
        {
            size_t deserialized = 0;

            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            value.resize(size);
            auto* data = static_cast<std::wstring::value_type*>(value.data());
            for (auto i = 0; i < size; i++) {
                uint32_t tempValue;
                stream.Read<uint32_t>(tempValue);
                data[i] = static_cast<std::wstring::value_type>(tempValue);
            }

            deserialized += size * sizeof(uint32_t);
            return deserialized;
        }
    };

    template <Serializable T>
    struct Serializer<std::optional<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::optional")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const std::optional<T>& value)
        {
            size_t serialized = 0;

            const bool hasValue = value.has_value();
            serialized += Serializer<bool>::Serialize(stream, hasValue);

            if (hasValue) {
                serialized += Serializer<T>::Serialize(stream, value.value());
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::optional<T>& value)
        {
            size_t deserialized = 0;

            value.reset();
            bool hasValue;
            deserialized += Serializer<bool>::Deserialize(stream, hasValue);

            if (hasValue) {
                T temp;
                deserialized += Serializer<T>::Deserialize(stream, temp);
                value.emplace(std::move(temp));
            }
            return deserialized;
        }
    };

    template <Serializable K, Serializable V>
    struct Serializer<std::pair<K, V>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::pair")
            + Serializer<K>::typeId
            + Serializer<V>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const std::pair<K, V>& value)
        {
            size_t serialized = 0;
            serialized += Serializer<K>::Serialize(stream, value.first);
            serialized += Serializer<V>::Serialize(stream, value.second);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::pair<K, V>& value)
        {
            size_t deserialized = 0;
            deserialized += Serializer<K>::Deserialize(stream, value.first);
            deserialized += Serializer<V>::Deserialize(stream, value.second);
            return deserialized;
        }
    };

    template <Serializable T, size_t N>
    struct Serializer<std::array<T, N>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::array")
            + Serializer<T>::typeId
            + N;

        static size_t Serialize(BinarySerializeStream& stream, const std::array<T, N>& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            for (const auto& element : value) {
                serialized += Serializer<T>::Serialize(stream, element);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::array<T, N>& value)
        {
            size_t deserialized = 0;

            for (auto& element : value) {
                element = T();
            }

            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);
            if (size != N) {
                return deserialized;
            }

            for (auto i = 0; i < size; i++) {
                T element;
                deserialized += Serializer<T>::Deserialize(stream, element);
                value[i] = std::move(element);
            }
            return deserialized;
        }
    };

    template <Serializable T>
    struct Serializer<std::vector<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::vector")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const std::vector<T>& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            for (auto i = 0; i < size; i++) {
                serialized += Serializer<T>::Serialize(stream, value[i]);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::vector<T>& value)
        {
            size_t deserialized = 0;

            value.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                T element;
                deserialized += Serializer<T>::Deserialize(stream, element);
                value.emplace_back(std::move(element));
            }
            return deserialized;
        }
    };

    template <Serializable T>
    struct Serializer<std::list<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::list")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const std::list<T>& value)
        {
            size_t serialized = 0;

            serialized += Serializer<uint64_t>::Serialize(stream, value.size());
            for (const auto& element : value) {
                serialized += Serializer<T>::Serialize(stream, element);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::list<T>& value)
        {
            size_t deserialized = 0;

            value.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            for (auto i = 0; i < size; i++) {
                T element;
                deserialized += Serializer<T>::Deserialize(stream, element);
                value.emplace_back(std::move(element));
            }
            return deserialized;
        }
    };

    template <Serializable T>
    struct Serializer<std::unordered_set<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::unordered_set")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const std::unordered_set<T>& value)
        {
            size_t serialized = Serializer<uint64_t>::Serialize(stream, value.size());
            for (const auto& element : value) {
                serialized += Serializer<T>::Serialize(stream, element);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::unordered_set<T>& value)
        {
            size_t deserialized = 0;

            value.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                T temp;
                deserialized += Serializer<T>::Deserialize(stream, temp);
                value.emplace(std::move(temp));
            }
            return deserialized;
        }
    };

    template <Serializable T>
    struct Serializer<std::set<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::set")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const std::set<T>& value)
        {
            size_t serialized = Serializer<uint64_t>::Serialize(stream, value.size());
            for (const auto& element : value) {
                serialized += Serializer<T>::Serialize(stream, element);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::set<T>& value)
        {
            size_t deserialized = 0;

            value.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            for (auto i = 0; i < size; i++) {
                T temp;
                deserialized += Serializer<T>::Deserialize(stream, temp);
                value.emplace(std::move(temp));
            }
            return deserialized;
        }
    };

    template <Serializable K, Serializable V>
    struct Serializer<std::unordered_map<K, V>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::unordered_map")
            + Serializer<K>::typeId
            + Serializer<V>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const std::unordered_map<K, V>& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            for (const auto& pair : value) {
                serialized += Serializer<std::pair<K, V>>::Serialize(stream, pair);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::unordered_map<K, V>& value)
        {
            size_t deserialized = 0;

            value.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                std::pair<K, V> pair;
                deserialized += Serializer<std::pair<K, V>>::Deserialize(stream, pair);
                value.emplace(std::move(pair));
            }
            return deserialized;
        }
    };

    template <Serializable K, Serializable V>
    struct Serializer<std::map<K, V>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::map")
            + Serializer<K>::typeId
            + Serializer<V>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const std::map<K, V>& value)
        {
            size_t serialized = 0;

            serialized += Serializer<uint64_t>::Serialize(stream, value.size());
            for (const auto& pair : value) {
                serialized += Serializer<std::pair<K, V>>::Serialize(stream, pair);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::map<K, V>& value)
        {
            size_t deserialized = 0;

            value.clear();
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            for (auto i = 0; i < size; i++) {
                std::pair<K, V> pair;
                deserialized += Serializer<std::pair<K, V>>::Deserialize(stream, pair);
                value.emplace(std::move(pair));
            }
            return deserialized;
        }
    };

    template <typename... T> struct TupleTypeId {};
    template <typename T> struct TupleTypeId<T> { static constexpr size_t value = HashUtils::StrCrc32("std::tuple"); };
    template <typename T, typename... T2> struct TupleTypeId<T, T2...> { static constexpr size_t value = Serializer<T>::typeId + TupleTypeId<T2...>::value; };

    template <Serializable... T>
    struct Serializer<std::tuple<T...>> {
        static constexpr size_t typeId = TupleTypeId<T...>::value;

        template <size_t... I>
        static size_t SerializeInternal(BinarySerializeStream& stream, const std::tuple<T...>& value, std::index_sequence<I...>)
        {
            size_t serialized = 0;
            std::initializer_list<int> { ([&]() -> void {
                serialized += Serializer<T>::Serialize(stream, std::get<I>(value));
            }(), 0)... };
            return serialized;
        }

        template <size_t... I>
        static size_t DeserializeInternal(BinaryDeserializeStream& stream, std::tuple<T...>& value, std::index_sequence<I...>)
        {
            size_t deserialized = 0;
            std::initializer_list<int> { ([&]() -> void {
                deserialized += Serializer<T>::Deserialize(stream, std::get<I>(value));
            }(), 0)... };
            return deserialized;
        }

        static size_t Serialize(BinarySerializeStream& stream, const std::tuple<T...>& value)
        {
            size_t serialized = 0;
            serialized += Serializer<uint64_t>::Serialize(stream, sizeof...(T));
            serialized += SerializeInternal(stream, value, std::make_index_sequence<sizeof...(T)> {});
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::tuple<T...>& value)
        {
            size_t deserialized = 0;

            value = {};
            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);
            deserialized += DeserializeInternal(stream, value, std::make_index_sequence<sizeof...(T)> {});
            return deserialized;
        }
    };

    template <>
    struct JsonSerializer<bool> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const bool& inValue)
        {
            outJsonValue.SetBool(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, bool& outValue)
        {
            if (!inJsonValue.IsBool()) {
                return;
            }
            outValue = inJsonValue.GetBool();
        }
    };

    template <typename... T> struct VariantTypeId {};
    template <typename T> struct VariantTypeId<T> { static constexpr size_t value = HashUtils::StrCrc32("std::variant"); };
    template <typename T, typename... T2> struct VariantTypeId<T, T2...> { static constexpr size_t value = Serializer<T>::typeId + VariantTypeId<T2...>::value; };

    template <Serializable... T>
    struct Serializer<std::variant<T...>> {
        static constexpr size_t typeId = VariantTypeId<T...>::value;

        static size_t Serialize(BinarySerializeStream& stream, const std::variant<T...>& value)
        {
            size_t serialized  = 0;
            serialized += Serializer<uint64_t>::Serialize(stream, value.index());
            std::visit([&](auto&& v) -> void { // NOLINT
                serialized += Serializer<std::decay_t<decltype(v)>>::Serialize(stream, v);
            }, value);
            return serialized;
        }

        template <size_t... I>
        static size_t DeserializeInternal(BinaryDeserializeStream& stream, std::variant<T...>& value, size_t aspectIndex, std::index_sequence<I...>)
        {
            size_t deserialized = 0;
            (void) std::initializer_list<int> { ([&]() -> void {
                if (I != aspectIndex) {
                    return;
                }

                T tempValue;
                deserialized += Serializer<T>::Deserialize(stream, tempValue);
                value = std::move(tempValue);
            }(), 0)... };
            return deserialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, std::variant<T...>& value)
        {
            size_t deserialized = 0;

            uint64_t index;
            deserialized += Serializer<uint64_t>::Deserialize(stream, index);
            deserialized += DeserializeInternal(stream, value, index, std::make_index_sequence<sizeof...(T)> {});
            return deserialized;
        }
    };

    template <>
    struct JsonSerializer<int8_t> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const int8_t& inValue)
        {
            outJsonValue.SetInt(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, int8_t& outValue)
        {
            if (!inJsonValue.IsInt()) {
                return;
            }
            outValue = static_cast<int8_t>(inJsonValue.GetInt());
        }
    };

    template <>
    struct JsonSerializer<uint8_t> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const uint8_t& inValue)
        {
            outJsonValue.SetUint(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, uint8_t& outValue)
        {
            if (!inJsonValue.IsUint()) {
                return;
            }
            outValue = static_cast<uint8_t>(inJsonValue.GetUint());
        }
    };

    template <>
    struct JsonSerializer<int16_t> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const int16_t& inValue)
        {
            outJsonValue.SetInt(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, int16_t& outValue)
        {
            if (!inJsonValue.IsInt()) {
                return;
            }
            outValue = static_cast<int16_t>(inJsonValue.GetInt());
        }
    };

    template <>
    struct JsonSerializer<uint16_t> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const uint16_t& inValue)
        {
            outJsonValue.SetUint(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, uint16_t& outValue)
        {
            if (!inJsonValue.IsUint()) {
                return;
            }
            outValue = static_cast<uint16_t>(inJsonValue.GetUint());
        }
    };

    template <>
    struct JsonSerializer<int32_t> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const int32_t& inValue)
        {
            outJsonValue.SetInt(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, int32_t& outValue)
        {
            if (!inJsonValue.IsInt()) {
                return;
            }
            outValue = inJsonValue.GetInt();
        }
    };

    template <>
    struct JsonSerializer<uint32_t> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const uint32_t& inValue)
        {
            outJsonValue.SetUint(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, uint32_t& outValue)
        {
            if (!inJsonValue.IsUint()) {
                return;
            }
            outValue = inJsonValue.GetUint();
        }
    };

    template <>
    struct JsonSerializer<int64_t> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const int64_t& inValue)
        {
            outJsonValue.SetInt64(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, int64_t& outValue)
        {
            if (!inJsonValue.IsInt64()) {
                return;
            }
            outValue = inJsonValue.GetInt64();
        }
    };

    template <>
    struct JsonSerializer<uint64_t> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const uint64_t& inValue)
        {
            outJsonValue.SetUint64(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, uint64_t& outValue)
        {
            if (!inJsonValue.IsUint64()) {
                return;
            }
            outValue = inJsonValue.GetUint64();
        }
    };

    template <>
    struct JsonSerializer<float> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const float& inValue)
        {
            outJsonValue.SetFloat(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, float& outValue)
        {
            if (!inJsonValue.IsFloat()) {
                return;
            }
            outValue = inJsonValue.GetFloat();
        }
    };

    template <>
    struct JsonSerializer<double> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const double& inValue)
        {
            outJsonValue.SetDouble(inValue);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, double& outValue)
        {
            if (!inJsonValue.IsDouble()) {
                return;
            }
            outValue = inJsonValue.GetDouble();
        }
    };

    template <>
    struct JsonSerializer<std::string> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::string& inValue)
        {
            outJsonValue.SetString(inValue.c_str(), inValue.length(), inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::string& outValue)
        {
            if (!inJsonValue.IsString()) {
                return;
            }
            outValue = std::string(inJsonValue.GetString(), inJsonValue.GetStringLength());
        }
    };

    template <>
    struct JsonSerializer<std::wstring> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::wstring& inValue)
        {
            const auto str = StringUtils::ToByteString(inValue);
            outJsonValue.SetString(str.c_str(), str.length(), inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::wstring& outValue)
        {
            if (!inJsonValue.IsString()) {
                return;
            }
            outValue = StringUtils::ToWideString(std::string(inJsonValue.GetString(), inJsonValue.GetStringLength()));
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<std::optional<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::optional<T>& inValue)
        {
            if (inValue.has_value()) {
                JsonSerializer<T>::JsonSerialize(outJsonValue, inAllocator, inValue.value());
            } else {
                outJsonValue.SetNull();
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::optional<T>& outValue)
        {
            if (inJsonValue.IsNull()) {
                outValue = {};
            } else {
                T value;
                JsonSerializer<T>::JsonDeserialize(inJsonValue, value);
                outValue = std::move(value);
            }
        }
    };

    template <JsonSerializable K, JsonSerializable V>
    struct JsonSerializer<std::pair<K, V>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::pair<K, V>& inValue)
        {
            outJsonValue.SetObject();

            rapidjson::Value jsonKey;
            JsonSerializer<K>::JsonSerialize(jsonKey, inAllocator, inValue.first);

            rapidjson::Value jsonValue;
            JsonSerializer<V>::JsonSerialize(jsonValue, inAllocator, inValue.second);

            outJsonValue.AddMember("key", jsonKey, inAllocator);
            outJsonValue.AddMember("value", jsonValue, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::pair<K, V>& outValue)
        {
            if (!inJsonValue.IsObject()) {
                return;
            }
            if (inJsonValue.HasMember("key")) {
                JsonSerializer<K>::JsonDeserialize(inJsonValue["key"], outValue.first);
            }
            if (inJsonValue.HasMember("value")) {
                JsonSerializer<V>::JsonDeserialize(inJsonValue["value"], outValue.second);
            }
        }
    };

    template <JsonSerializable T, size_t N>
    struct JsonSerializer<std::array<T, N>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::array<T, N>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inValue.size(), inAllocator);
            for (const auto& element : inValue) {
                rapidjson::Value jsonElement;
                JsonSerializer<T>::JsonSerialize(jsonElement, inAllocator, element);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::array<T, N>& outValue)
        {
            for (auto& element : outValue) {
                element = T();
            }

            if (!inJsonValue.IsArray() || inJsonValue.Size() != N) {
                return;
            }
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                T element;
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], element);
                outValue[i] = std::move(element);
            }
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<std::vector<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::vector<T>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inValue.size(), inAllocator);
            for (const auto& element : inValue) {
                rapidjson::Value jsonElement;
                JsonSerializer<T>::JsonSerialize(jsonElement, inAllocator, element);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::vector<T>& outValue)
        {
            outValue.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            outValue.reserve(inJsonValue.Size());
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                T element;
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], element);
                outValue.emplace_back(std::move(element));
            }
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<std::list<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::list<T>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inValue.size(), inAllocator);
            for (const auto& element : inValue) {
                rapidjson::Value jsonElement;
                JsonSerializer<T>::JsonSerialize(jsonElement, inAllocator, element);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::list<T>& outValue)
        {
            outValue.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                T element;
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], element);
                outValue.emplace_back(std::move(element));
            }
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<std::unordered_set<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::unordered_set<T>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inValue.size(), inAllocator);
            for (const auto& element : inValue) {
                rapidjson::Value jsonElement;
                JsonSerializer<T>::JsonSerialize(jsonElement, inAllocator, element);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::unordered_set<T>& outValue)
        {
            outValue.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            outValue.reserve(inJsonValue.Size());
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                T element;
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], element);
                outValue.emplace(std::move(element));
            }
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<std::set<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::set<T>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inValue.size(), inAllocator);
            for (const auto& element : inValue) {
                rapidjson::Value jsonElement;
                JsonSerializer<T>::JsonSerialize(jsonElement, inAllocator, element);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::set<T>& outValue)
        {
            outValue.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                T element;
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], element);
                outValue.emplace(std::move(element));
            }
        }
    };

    template <JsonSerializable K, JsonSerializable V>
    struct JsonSerializer<std::unordered_map<K, V>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::unordered_map<K, V>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inValue.size(), inAllocator);
            for (const auto& pair : inValue) {
                rapidjson::Value jsonElement;
                JsonSerializer<std::pair<K, V>>::JsonSerialize(jsonElement, inAllocator, pair);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::unordered_map<K, V>& outValue)
        {
            outValue.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            outValue.reserve(inJsonValue.Size());
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                std::pair<K, V> pair;
                JsonSerializer<std::pair<K, V>>::JsonDeserialize(inJsonValue[i], pair);
                outValue.emplace(std::move(pair));
            }
        }
    };

    template <JsonSerializable K, JsonSerializable V>
    struct JsonSerializer<std::map<K, V>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::map<K, V>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(inValue.size(), inAllocator);
            for (const auto& pair : inValue) {
                rapidjson::Value jsonElement;
                JsonSerializer<std::pair<K, V>>::JsonSerialize(jsonElement, inAllocator, pair);
                outJsonValue.PushBack(jsonElement, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::map<K, V>& outValue)
        {
            outValue.clear();

            if (!inJsonValue.IsArray()) {
                return;
            }
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                std::pair<K, V> pair;
                JsonSerializer<std::pair<K, V>>::JsonDeserialize(inJsonValue[i], pair);
                outValue.emplace(std::move(pair));
            }
        }
    };

    template <JsonSerializable... T>
    struct JsonSerializer<std::tuple<T...>> {
        template <size_t... I>
        static void JsonSerializeInternal(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::tuple<T...>& inValue, std::index_sequence<I...>)
        {
            std::initializer_list<int> { ([&]() -> void {
                const auto key = std::to_string(I);

                rapidjson::Value jsonKey;
                JsonSerializer<std::string>::JsonSerialize(jsonKey, inAllocator, key);

                rapidjson::Value jsonValue;
                JsonSerializer<T>::JsonSerialize(jsonValue, inAllocator, std::get<I>(inValue));

                outJsonValue.AddMember(jsonKey, jsonValue, inAllocator);
            }(), 0)... };
        }

        template <size_t... I>
        static void JsonDeserializeInternal(const rapidjson::Value& inJsonValue, std::tuple<T...>& outValue, std::index_sequence<I...>)
        {
            std::initializer_list<int> { ([&]() -> void {
                const auto key = std::to_string(I);
                if (!inJsonValue.HasMember(key.c_str())) {
                    return;
                }
                JsonSerializer<T>::JsonDeserialize(inJsonValue[key.c_str()], std::get<I>(outValue));
            }(), 0)... };
        }

        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::tuple<T...>& inValue)
        {
            outJsonValue.SetObject();
            JsonSerializeInternal(outJsonValue, inAllocator, inValue, std::make_index_sequence<sizeof...(T)>());
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::tuple<T...>& outValue)
        {
            outValue = {};

            if (!inJsonValue.IsObject()) {
                return;
            }
            JsonDeserializeInternal(inJsonValue, outValue, std::make_index_sequence<sizeof...(T)>());
        }
    };

    template <JsonSerializable... T>
    struct JsonSerializer<std::variant<T...>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const std::variant<T...>& inValue)
        {
            rapidjson::Value typeValue;
            JsonSerializer<uint64_t>::JsonSerialize(typeValue, inAllocator, inValue.index());

            rapidjson::Value contentValue;
            std::visit([&](auto&& v) -> void {
                JsonSerializer<std::decay_t<decltype(v)>>::JsonSerialize(contentValue, inAllocator, v);
            }, inValue);

            outJsonValue.SetObject();
            outJsonValue.AddMember("type", typeValue, inAllocator);
            outJsonValue.AddMember("content", contentValue, inAllocator);
        }

        template <size_t... I>
        static void JsonDeserializeInternal(const rapidjson::Value& inContentJsonValue, std::variant<T...>& outValue, size_t inAspectIndex, std::index_sequence<I...>)
        {
            (void) std::initializer_list<int> { ([&]() -> void {
                if (I != inAspectIndex) {
                    return;
                }

                T temp;
                JsonSerializer<T>::JsonDeserialize(inContentJsonValue, temp);
                outValue = std::move(temp);
            }(), 0)... };
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::variant<T...>& outValue)
        {
            if (!inJsonValue.HasMember("type") || !inJsonValue.HasMember("content")) {
                return;
            }

            uint64_t aspectIndex;
            JsonSerializer<uint64_t>::JsonDeserialize(inJsonValue["type"], aspectIndex);
            JsonDeserializeInternal(inJsonValue["content"], outValue, aspectIndex, std::make_index_sequence<sizeof...(T)> {});
        }
    };
}
