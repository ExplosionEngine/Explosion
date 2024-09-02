//
// Created by johnk on 2023/7/13.
//

#pragma once

#include <fstream>
#include <string>
#include <optional>
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

    protected:
        SerializeStream();
    };

    class DeserializeStream {
    public:
        NonCopyable(DeserializeStream);
        virtual ~DeserializeStream();

        virtual void Read(void* data, size_t size) = 0;

    protected:
        DeserializeStream();
    };

    class BinaryFileSerializeStream final : public SerializeStream {
    public:
        NonCopyable(BinaryFileSerializeStream)
        explicit BinaryFileSerializeStream(const std::string& inFileName);
        ~BinaryFileSerializeStream() override;
        void Write(const void* data, size_t size) override;
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

    template <Serializable T>
    struct TypeIdSerializer {
        static void Serialize(SerializeStream& stream);
        static bool Deserialize(DeserializeStream& stream);
    };

    template <typename T> void Serialize(SerializeStream& inStream, const T& inValue);
    template <typename T> void Deserialize(DeserializeStream& inStream, T& inValue);

    template <typename T> struct JsonValueConverter {};
    template <typename T> concept JsonValueConvertible = requires(T inValue, rapidjson::Document::AllocatorType& inAllocator, const rapidjson::Value& inJsonValue)
    {
        { JsonValueConverter<T>::ToJsonValue(inValue, inAllocator) } -> std::convertible_to<rapidjson::Value>;
        { JsonValueConverter<T>::FromJsonValue(inJsonValue) } -> std::same_as<T>;
    };

    template <typename T> rapidjson::Value ToJsonValue(const T& inValue, rapidjson::Document::AllocatorType& inAllocator);
    template <typename T> T FromJsonValue(const rapidjson::Value& inJsonValue);
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
    void Deserialize(DeserializeStream& inStream, T& inValue)
    {
        if constexpr (Serializable<T>) {
            Serializer<T>::Deserialize(inStream, inValue);
        } else {
            QuickFailWithReason("your type is not support serialization");
        }
    }

    template <typename T>
    rapidjson::Value ToJsonValue(const T& inValue, rapidjson::Document::AllocatorType& inAllocator)
    {
        if constexpr (JsonValueConvertible<T>) {
            return JsonValueConverter<T>::ToJsonValue(inValue, inAllocator);
        } else {
            QuickFailWithReason("your type is not support json serialization");
            return {};
        }
    }

    template <typename T>
    T FromJsonValue(const rapidjson::Value& inJsonValue)
    {
        if constexpr (JsonValueConvertible<T>) {
            return JsonValueConverter<T>::FromJsonValue(inJsonValue);
        } else {
            QuickFailWithReason("your type is not support json serialization");
            return {};
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
    struct JsonValueConverter<bool> {
        static rapidjson::Value ToJsonValue(const bool& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static bool FromJsonValue(const rapidjson::Value& inValue)
        {
            return inValue.GetBool();
        }
    };

    template <>
    struct JsonValueConverter<int8_t> {
        static rapidjson::Value ToJsonValue(const int8_t& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static int8_t FromJsonValue(const rapidjson::Value& inValue)
        {
            return static_cast<int8_t>(inValue.GetInt());
        }
    };

    template <>
    struct JsonValueConverter<uint8_t> {
        static rapidjson::Value ToJsonValue(const uint8_t& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static uint8_t FromJsonValue(const rapidjson::Value& inValue)
        {
            return static_cast<uint8_t>(inValue.GetUint());
        }
    };

    template <>
    struct JsonValueConverter<int16_t> {
        static rapidjson::Value ToJsonValue(const int16_t& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static int16_t FromJsonValue(const rapidjson::Value& inValue)
        {
            return static_cast<int16_t>(inValue.GetInt());
        }
    };

    template <>
    struct JsonValueConverter<uint16_t> {
        static rapidjson::Value ToJsonValue(const uint16_t& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static uint16_t FromJsonValue(const rapidjson::Value& inValue)
        {
            return static_cast<uint16_t>(inValue.GetUint());
        }
    };

    template <>
    struct JsonValueConverter<int32_t> {
        static rapidjson::Value ToJsonValue(const int32_t& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static int32_t FromJsonValue(const rapidjson::Value& inValue)
        {
            return inValue.GetInt();
        }
    };

    template <>
    struct JsonValueConverter<uint32_t> {
        static rapidjson::Value ToJsonValue(const uint32_t& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static uint32_t FromJsonValue(const rapidjson::Value& inValue)
        {
            return inValue.GetUint();
        }
    };

    template <>
    struct JsonValueConverter<int64_t> {
        static rapidjson::Value ToJsonValue(const int64_t& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static int64_t FromJsonValue(const rapidjson::Value& inValue)
        {
            return inValue.GetInt64();
        }
    };

    template <>
    struct JsonValueConverter<uint64_t> {
        static rapidjson::Value ToJsonValue(const uint64_t& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static uint64_t FromJsonValue(const rapidjson::Value& inValue)
        {
            return inValue.GetUint64();
        }
    };

    template <>
    struct JsonValueConverter<float> {
        static rapidjson::Value ToJsonValue(const float& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static float FromJsonValue(const rapidjson::Value& inValue)
        {
            return inValue.GetFloat();
        }
    };

    template <>
    struct JsonValueConverter<double> {
        static rapidjson::Value ToJsonValue(const double& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue);
        }

        static double FromJsonValue(const rapidjson::Value& inValue)
        {
            return inValue.GetDouble();
        }
    };

    template <>
    struct JsonValueConverter<std::string> {
        static rapidjson::Value ToJsonValue(const std::string& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            return rapidjson::Value(inValue.c_str(), inValue.length()); // NOLINT
        }

        static std::string FromJsonValue(const rapidjson::Value& inValue)
        {
            return { inValue.GetString(), inValue.GetStringLength() };
        }
    };

    template <JsonValueConvertible T>
    struct JsonValueConverter<std::optional<T>> {
        static rapidjson::Value ToJsonValue(const std::optional<T>& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            if (inValue.has_value()) {
                return JsonValueConverter<T>::ToJsonValue(inValue.value(), inAllocator);
            }
            return rapidjson::Value(rapidjson::kNullType);
        }

        static std::optional<T> FromJsonValue(const rapidjson::Value& inValue)
        {
            if (inValue.IsNull()) {
                return {};
            }
            return JsonValueConverter<T>::FromJsonValue(inValue);
        }
    };

    template <JsonValueConvertible K, JsonValueConvertible V>
    struct JsonValueConverter<std::pair<K, V>> {
        static rapidjson::Value ToJsonValue(const std::pair<K, V>& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            rapidjson::Value result(rapidjson::kObjectType);
            result.AddMember("key", JsonValueConverter<K>::ToJsonValue(inValue.first, inAllocator), inAllocator);
            result.AddMember("value", JsonValueConverter<V>::ToJsonValue(inValue.second, inAllocator), inAllocator);
            return result;
        }

        static std::pair<K, V> FromJsonValue(const rapidjson::Value& inValue)
        {
            return {
                JsonValueConverter<K>::FromJsonValue(inValue["key"]),
                JsonValueConverter<V>::FromJsonValue(inValue["value"])
            };
        }
    };

    template <JsonValueConvertible T>
    struct JsonValueConverter<std::vector<T>> {
        static rapidjson::Value ToJsonValue(const std::vector<T>& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            rapidjson::Value result(rapidjson::kArrayType);
            for (const auto& element : inValue) {
                result.PushBack(JsonValueConverter<T>::ToJsonValue(element, inAllocator), inAllocator);
            }
            return result;
        }

        static std::vector<T> FromJsonValue(const rapidjson::Value& inValue)
        {
            std::vector<T> result;
            for (auto i = 0; i < inValue.Size(); i++) {
                result.emplace_back(JsonValueConverter<T>::FromJsonValue(inValue[i]));
            }
            return result;
        }
    };

    template <JsonValueConvertible T>
    struct JsonValueConverter<std::unordered_set<T>> {
        static rapidjson::Value ToJsonValue(const std::unordered_set<T>& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            rapidjson::Value result(rapidjson::kArrayType);
            for (const auto& element : inValue) {
                result.PushBack(JsonValueConverter<T>::ToJsonValue(element, inAllocator), inAllocator);
            }
            return result;
        }

        static std::unordered_set<T> FromJsonValue(const rapidjson::Value& inValue)
        {
            std::unordered_set<T> result;
            for (auto i = 0; i < inValue.Size(); i++) {
                result.emplace(JsonValueConverter<T>::FromJsonValue(inValue[i]));
            }
            return result;
        }
    };

    template <JsonValueConvertible K, JsonValueConvertible V>
    struct JsonValueConverter<std::unordered_map<K, V>> {
        static rapidjson::Value ToJsonValue(const std::unordered_map<K, V>& inValue, rapidjson::Document::AllocatorType& inAllocator)
        {
            rapidjson::Value result(rapidjson::kArrayType);
            for (const auto& pair : inValue) {
                result.PushBack(JsonValueConverter<std::pair<K, V>>::ToJsonValue(pair, inAllocator), inAllocator);
            }
            return result;
        }

        static std::unordered_map<K, V> FromJsonValue(const rapidjson::Value& inValue)
        {
            std::unordered_map<K, V> result;
            for (auto i = 0; i < inValue.Size(); i++) {
                result.emplace(JsonValueConverter<std::pair<K, V>>::FromJsonValue(inValue[i]));
            }
            return result;
        }
    };
}
