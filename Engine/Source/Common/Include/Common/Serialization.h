//
// Created by johnk on 2023/7/13.
//

#pragma once

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

        void WriteTyped(const void* data, size_t size, uint32_t typeCrc);
        virtual void Write(const void* data, size_t size) = 0;
        virtual void Seek(int64_t offset) = 0;

    protected:
        SerializeStream();
    };

    class DeserializeStream {
    public:
        NonCopyable(DeserializeStream);
        virtual ~DeserializeStream();

        bool ReadTyped(void* data, size_t size, uint32_t typeCrc);
        virtual void Read(void* data, size_t size) = 0;
        virtual void Seek(int64_t offset) = 0;

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
        void Close();

    private:
        std::ifstream file;
    };

    class ByteSerializeStream final : public SerializeStream {
    public:
        NonCopyable(ByteSerializeStream)
        explicit ByteSerializeStream(std::vector<uint8_t>& inBytes, size_t pointerBegin = 0);
        ~ByteSerializeStream() override;
        void Write(const void* data, size_t size) override;
        void Seek(int64_t offset) override;

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

    private:
        size_t pointer;
        const std::vector<uint8_t>& bytes;
    };

    template <typename T> struct Serializer {};
    template <typename T> concept Serializable = requires(T inValue, SerializeStream& serializeStream, DeserializeStream& deserializeStream)
    {
        { Serializer<T>::typeId } -> std::convertible_to<uint32_t>;
        Serializer<T>::Serialize(serializeStream, inValue);
        Serializer<T>::Deserialize(deserializeStream, inValue);
    };

    // TODO remove this or find a better way to perform check
    template <Serializable T>
    struct TypeIdSerializer {
        static void Serialize(SerializeStream& stream);
        static bool Deserialize(DeserializeStream& stream);
    };

    template <typename T> void Serialize(SerializeStream& inStream, const T& inValue);
    template <typename T> bool Deserialize(DeserializeStream& inStream, T& inValue);

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
        static constexpr uint32_t typeId = HashUtils::StrCrc32(#typeName); \
        \
        static void Serialize(SerializeStream& stream, const typeName& value) \
        { \
            TypeIdSerializer<typeName>::Serialize(stream); \
            stream.Write(&value, sizeof(typeName)); \
        } \
        \
        static bool Deserialize(DeserializeStream& stream, typeName& value) \
        { \
            if (!TypeIdSerializer<typeName>::Deserialize(stream)) { \
                return false;\
            } \
            stream.Read(&value, sizeof(typeName)); \
            return true; \
        } \
    }; \

namespace Common {
    template <Serializable T>
    void TypeIdSerializer<T>::Serialize(SerializeStream& stream)
    {
        const uint32_t typeId = Serializer<T>::typeId;
        stream.Write(&typeId, sizeof(uint32_t));
    }

    template <Serializable T>
    bool TypeIdSerializer<T>::Deserialize(DeserializeStream& stream)
    {
        uint32_t typeId;
        stream.Read(&typeId, sizeof(uint32_t));
        return typeId == Serializer<T>::typeId;
    }

    template <typename T>
    void Serialize(SerializeStream& inStream, const T& inValue)
    {
        if constexpr (Serializable<T>) {
            Serializer<T>::Serialize(inStream, inValue);
        } else {
            QuickFailWithReason("your type is not support serialization");
        }
    }

    template <typename T>
    bool Deserialize(DeserializeStream& inStream, T& inValue)
    {
        if constexpr (Serializable<T>) {
            return Serializer<T>::Deserialize(inStream, inValue);
        } else {
            QuickFailWithReason("your type is not support serialization");
            return false;
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
            return JsonSerializer<T>::JsonDeserialize(inJsonValue, outValue);
        } else {
            QuickFailWithReason("your type is not support json serialization");
        }
    }

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
        static constexpr uint32_t typeId = HashUtils::StrCrc32("string");

        static void Serialize(SerializeStream& stream, const std::string& value)
        {
            TypeIdSerializer<std::string>::Serialize(stream);

            const uint64_t size = value.size();
            Serializer<uint64_t>::Serialize(stream, size);
            stream.Write(value.data(), value.size());
        }

        static bool Deserialize(DeserializeStream& stream, std::string& value)
        {
            if (!TypeIdSerializer<std::string>::Deserialize(stream)) {
                return false;
            }

            uint64_t size;
            Serializer<uint64_t>::Deserialize(stream, size);
            value.resize(size);
            stream.Read(value.data(), size);
            return true;
        }
    };

    template <Serializable T>
    struct Serializer<std::optional<T>> {
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("std::optional")
            + Serializer<T>::typeId;

        static void Serialize(SerializeStream& stream, const std::optional<T>& value)
        {
            TypeIdSerializer<std::optional<T>>::Serialize(stream);

            const bool hasValue = value.has_value();
            Serializer<bool>::Serialize(stream, hasValue);

            if (hasValue) {
                Serializer<T>::Serialize(stream, value.value());
            }
        }

        static bool Deserialize(DeserializeStream& stream, std::optional<T>& value)
        {
            if (!TypeIdSerializer<std::optional<T>>::Deserialize(stream)) {
                return false;
            }

            value.reset();
            bool hasValue;
            Serializer<bool>::Deserialize(stream, hasValue);

            if (hasValue) {
                T temp;
                Serializer<T>::Deserialize(stream, temp);
                value.emplace(std::move(temp));
            }
            return true;
        }
    };

    template <Serializable K, Serializable V>
    struct Serializer<std::pair<K, V>> {
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("std::pair")
            + Serializer<K>::typeId
            + Serializer<V>::typeId;

        static void Serialize(SerializeStream& stream, const std::pair<K, V>& value)
        {
            TypeIdSerializer<std::pair<K, V>>::Serialize(stream);

            Serializer<K>::Serialize(stream, value.first);
            Serializer<V>::Serialize(stream, value.second);
        }

        static bool Deserialize(DeserializeStream& stream, std::pair<K, V>& value)
        {
            if (!TypeIdSerializer<std::pair<K, V>>::Deserialize(stream)) {
                return false;
            }

            Serializer<K>::Deserialize(stream, value.first);
            Serializer<V>::Deserialize(stream, value.second);
            return true;
        }
    };

    template <Serializable T>
    struct Serializer<std::vector<T>> {
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("std::vector")
            + Serializer<T>::typeId;

        static void Serialize(SerializeStream& stream, const std::vector<T>& value)
        {
            TypeIdSerializer<std::vector<T>>::Serialize(stream);

            const uint64_t size = value.size();
            Serializer<uint64_t>::Serialize(stream, size);

            for (auto i = 0; i < size; i++) {
                Serializer<T>::Serialize(stream, value[i]);
            }
        }

        static bool Deserialize(DeserializeStream& stream, std::vector<T>& value)
        {
            if (!TypeIdSerializer<std::vector<T>>::Deserialize(stream)) {
                return false;
            }

            value.clear();

            uint64_t size;
            Serializer<uint64_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                T element;
                Serializer<T>::Deserialize(stream, element);
                value.emplace_back(std::move(element));
            }
            return true;
        }
    };

    template <Serializable T>
    struct Serializer<std::unordered_set<T>> {
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("std::unordered_set")
            + Serializer<T>::typeId;

        static void Serialize(SerializeStream& stream, const std::unordered_set<T>& value)
        {
            TypeIdSerializer<T>::Serialize(stream);

            const uint64_t size = value.size();
            Serializer<uint64_t>::Serialize(stream, size);

            for (const auto& element : value) {
                Serializer<T>::Serialize(stream, element);
            }
        }

        static bool Deserialize(DeserializeStream& stream, std::unordered_set<T>& value)
        {
            if (!TypeIdSerializer<T>::Deserialize(stream)) {
                return false;
            }

            value.clear();

            uint64_t size;
            Serializer<uint64_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                T temp;
                Serializer<T>::Deserialize(stream, temp);
                value.emplace(std::move(temp));
            }
            return true;
        }
    };

    template <Serializable K, Serializable V>
    struct Serializer<std::unordered_map<K, V>> {
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("std::unordered_map")
            + Serializer<K>::typeId
            + Serializer<V>::typeId;

        static void Serialize(SerializeStream& stream, const std::unordered_map<K, V>& value)
        {
            TypeIdSerializer<std::unordered_map<K, V>>::Serialize(stream);

            const uint64_t size = value.size();
            Serializer<uint64_t>::Serialize(stream, size);

            for (const auto& pair : value) {
                Serializer<K>::Serialize(stream, pair.first);
                Serializer<V>::Serialize(stream, pair.second);
            }
        }

        static bool Deserialize(DeserializeStream& stream, std::unordered_map<K, V>& value)
        {
            if (!TypeIdSerializer<std::unordered_map<K, V>>::Deserialize(stream)) {
                return false;
            }

            value.clear();

            uint64_t size;
            Serializer<uint64_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                std::pair<K, V> pair;
                Serializer<K>::Deserialize(stream, pair.first);
                Serializer<V>::Deserialize(stream, pair.second);
                value.emplace(std::move(pair));
            }
            return true;
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
            JsonSerializer<K>::JsonDeserialize(inJsonValue["key"], outValue.first);
            JsonSerializer<V>::JsonDeserialize(inJsonValue["value"], outValue.second);
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
            outValue.resize(inJsonValue.Size());
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], outValue[i]);
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
            outValue.reserve(inJsonValue.Size());
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                std::pair<K, V> pair;
                JsonSerializer<std::pair<K, V>>::JsonDeserialize(inJsonValue[i], pair);
                outValue.emplace(std::move(pair));
            }
        }
    };

    // TODO std::array
    // TODO std::set
    // TODO std::map
    // TODO std::wstring
}
