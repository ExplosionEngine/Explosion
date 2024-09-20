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
#include <unordered_set>
#include <unordered_map>

#include <rapidjson/document.h>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/Hash.h>

namespace Common {
    class SerializeStream {
    public:
        NonCopyable(SerializeStream)
        virtual ~SerializeStream();

        virtual void Write(const void* data, size_t size) = 0;
        virtual void Seek(int64_t offset) = 0;
        virtual size_t Loc() = 0;

    protected:
        SerializeStream();
    };

    class DeserializeStream {
    public:
        NonCopyable(DeserializeStream);
        virtual ~DeserializeStream();

        virtual void Read(void* data, size_t size) = 0;
        virtual void Seek(int64_t offset) = 0;
        virtual size_t Loc() = 0;

    protected:
        DeserializeStream();
    };

    class BinaryFileSerializeStream final : public SerializeStream {
    public:
        NonCopyable(BinaryFileSerializeStream)
        explicit BinaryFileSerializeStream(const std::string& inFileName);
        ~BinaryFileSerializeStream() override;
        void Write(const void* data, size_t size) override;
        void Seek(int64_t offset) override;
        size_t Loc() override;
        void Close();

    private:
        std::ofstream file;
    };

    class BinaryFileDeserializeStream final : public DeserializeStream {
    public:
        NonCopyable(BinaryFileDeserializeStream)
        explicit BinaryFileDeserializeStream(const std::string& inFileName);
        ~BinaryFileDeserializeStream() override;
        void Read(void* data, size_t size) override;
        void Seek(int64_t offset) override;
        size_t Loc() override;
        void Close();

    private:
        std::ifstream file;
        size_t fileSize;
    };

    class ByteSerializeStream final : public SerializeStream {
    public:
        NonCopyable(ByteSerializeStream)
        explicit ByteSerializeStream(std::vector<uint8_t>& inBytes, size_t pointerBegin = 0);
        ~ByteSerializeStream() override;
        void Write(const void* data, size_t size) override;
        void Seek(int64_t offset) override;
        size_t Loc() override;

    private:
        size_t pointer;
        std::vector<uint8_t>& bytes;
    };

    class ByteDeserializeStream final : public DeserializeStream {
    public:
        NonCopyable(ByteDeserializeStream)
        explicit ByteDeserializeStream(const std::vector<uint8_t>& inBytes, size_t pointerBegin = 0);
        ~ByteDeserializeStream() override;
        void Read(void* data, size_t size) override;
        void Seek(int64_t offset) override;
        size_t Loc() override;

    private:
        size_t pointer;
        const std::vector<uint8_t>& bytes;
    };

    template <typename T> struct Serializer {};
    template <typename T> concept Serializable = requires(T inValue, SerializeStream& serializeStream, DeserializeStream& deserializeStream)
    {
        { Serializer<T>::typeId } -> std::convertible_to<uint32_t>;
        { Serializer<T>::Serialize(serializeStream, inValue) } -> std::convertible_to<size_t>;
        { Serializer<T>::Deserialize(deserializeStream, inValue) } -> std::convertible_to<size_t>;
    };

    template <Serializable T> struct FieldSerializer;

    template <typename T> size_t Serialize(SerializeStream& inStream, const T& inValue);
    template <typename T> std::pair<bool, size_t> Deserialize(DeserializeStream& inStream, T& inValue);

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
}

#define IMPL_BASIC_TYPE_SERIALIZER(typeName) \
    template <> \
    struct Serializer<typeName> { \
        static constexpr size_t typeId = HashUtils::StrCrc32(#typeName); \
        \
        static size_t Serialize(SerializeStream& stream, const typeName& value) \
        { \
            stream.Write(&value, sizeof(typeName)); \
            return sizeof(typeName); \
        } \
        \
        static size_t Deserialize(DeserializeStream& stream, typeName& value) \
        { \
            stream.Read(&value, sizeof(typeName)); \
            return sizeof(typeName); \
        } \
    }; \

namespace Common {
    template <typename T>
    size_t Serialize(SerializeStream& inStream, const T& inValue)
    {
        if constexpr (Serializable<T>) {
            return FieldSerializer<T>::Serialize(inStream, inValue);
        } else {
            QuickFailWithReason("your type is not support serialization");
            return 0;
        }
    }

    template <typename T>
    std::pair<bool, size_t> Deserialize(DeserializeStream& inStream, T& inValue)
    {
        if constexpr (Serializable<T>) {
            return FieldSerializer<T>::Deserialize(inStream, inValue);
        } else {
            QuickFailWithReason("your type is not support serialization");
            return { false, 0 };
        }
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

    template <Serializable T>
    struct FieldSerializer {
        struct Header {
            size_t typeId;
            size_t contentSize;
        };

        static size_t Serialize(SerializeStream& stream, const T& value)
        {
            Header header;
            header.typeId = Serializer<T>::typeId;

            stream.Seek(sizeof(Header));
            header.contentSize = Serializer<T>::Serialize(stream, value);
            stream.Seek(-static_cast<int64_t>(sizeof(Header)) - static_cast<int64_t>(header.contentSize));
            stream.Write(&header, sizeof(Header));
            stream.Seek(header.contentSize);
            return sizeof(Header) + header.contentSize;
        }

        static std::pair<bool, size_t> Deserialize(DeserializeStream& stream, T& value)
        {
            Header header {};
            stream.Read(&header, sizeof(Header));

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
        static constexpr size_t typeId = HashUtils::StrCrc32("string");

        static size_t Serialize(SerializeStream& stream, const std::string& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            stream.Write(value.data(), value.size());
            serialized += size;
            return serialized;
        }

        static size_t Deserialize(DeserializeStream& stream, std::string& value)
        {
            size_t deserialized = 0;

            uint64_t size;
            deserialized += Serializer<uint64_t>::Deserialize(stream, size);

            value.resize(size);
            stream.Read(value.data(), size);
            deserialized += size;
            return deserialized;
        }
    };

    template <Serializable T>
    struct Serializer<std::optional<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::optional")
            + Serializer<T>::typeId;

        static size_t Serialize(SerializeStream& stream, const std::optional<T>& value)
        {
            size_t serialized = 0;

            const bool hasValue = value.has_value();
            serialized += Serializer<bool>::Serialize(stream, hasValue);

            if (hasValue) {
                serialized += Serializer<T>::Serialize(stream, value.value());
            }
            return serialized;
        }

        static size_t Deserialize(DeserializeStream& stream, std::optional<T>& value)
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

        static size_t Serialize(SerializeStream& stream, const std::pair<K, V>& value)
        {
            return Serializer<K>::Serialize(stream, value.first)
                + Serializer<V>::Serialize(stream, value.second);
        }

        static size_t Deserialize(DeserializeStream& stream, std::pair<K, V>& value)
        {
            return Serializer<K>::Deserialize(stream, value.first)
                + Serializer<V>::Deserialize(stream, value.second);
        }
    };

    template <Serializable T, size_t N>
    struct Serializer<std::array<T, N>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::array")
            + Serializer<T>::typeId
            + N;

        static size_t Serialize(SerializeStream& stream, const std::array<T, N>& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            for (const auto& element : value) {
                serialized += Serializer<T>::Serialize(stream, element);
            }
            return serialized;
        }

        static size_t Deserialize(DeserializeStream& stream, std::array<T, N>& value)
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

        static size_t Serialize(SerializeStream& stream, const std::vector<T>& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            for (auto i = 0; i < size; i++) {
                serialized += Serializer<T>::Serialize(stream, value[i]);
            }
            return serialized;
        }

        static size_t Deserialize(DeserializeStream& stream, std::vector<T>& value)
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
    struct Serializer<std::unordered_set<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::unordered_set")
            + Serializer<T>::typeId;

        static size_t Serialize(SerializeStream& stream, const std::unordered_set<T>& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            for (const auto& element : value) {
                serialized += Serializer<T>::Serialize(stream, element);
            }
            return serialized;
        }

        static size_t Deserialize(DeserializeStream& stream, std::unordered_set<T>& value)
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

    template <Serializable K, Serializable V>
    struct Serializer<std::unordered_map<K, V>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("std::unordered_map")
            + Serializer<K>::typeId
            + Serializer<V>::typeId;

        static size_t Serialize(SerializeStream& stream, const std::unordered_map<K, V>& value)
        {
            size_t serialized = 0;

            const uint64_t size = value.size();
            serialized += Serializer<uint64_t>::Serialize(stream, size);

            for (const auto& pair : value) {
                serialized += Serializer<std::pair<K, V>>::Serialize(stream, pair);
            }
            return serialized;
        }

        static size_t Deserialize(DeserializeStream& stream, std::unordered_map<K, V>& value)
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
            outJsonValue.SetString(inValue.c_str(), inValue.length());
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, std::string& outValue)
        {
            if (!inJsonValue.IsString()) {
                return;
            }
            outValue = std::string(inJsonValue.GetString(), inJsonValue.GetStringLength());
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

    // TODO std::wstring
    // TODO std::set
    // TODO std::map
    // TODO std::tuple
    // TODO std::queue
    // TODO std::stack
    // TODO std::list
}
