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

    private:
        std::ofstream file;
    };

    class BinaryFileDeserializeStream final : public DeserializeStream {
    public:
        NonCopyable(BinaryFileDeserializeStream)
        explicit BinaryFileDeserializeStream(const std::string& inFileName);
        ~BinaryFileDeserializeStream() override;
        void Read(void* data, size_t size) override;

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

    template <typename T>
    struct Serializer {
        static constexpr bool serializable = false;

        static void Serialize(SerializeStream&, const T&)
        {
            Unimplement();
        }

        static bool Deserialize(DeserializeStream&, T&)
        {
            Unimplement();
            return false;
        }
    };

    template <typename T>
    requires Serializer<T>::serializable
    struct TypeIdSerializer {
        static void Serialize(SerializeStream& stream)
        {
            const uint32_t typeId = Serializer<T>::typeId;
            stream.Write(&typeId, sizeof(uint32_t));
        }

        static bool Deserialize(DeserializeStream& stream)
        {
            uint32_t typeId;
            stream.Read(&typeId, sizeof(uint32_t));
            return typeId == Serializer<T>::typeId;
        }
    };
}

#define IMPL_BASIC_TYPE_SERIALIZER(typeName) \
    template <> \
    struct Serializer<typeName> { \
        static constexpr bool serializable = true; \
        static constexpr uint32_t typeId = Common::HashUtils::StrCrc32(#typeName); \
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
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = Common::HashUtils::StrCrc32("string");

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

    template <typename T>
    requires Serializer<T>::serializable
    struct Serializer<std::optional<T>> {
        static constexpr bool serializable = true;
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

    template <typename K, typename V>
    requires Serializer<K>::serializable && Serializer<V>::serializable
    struct Serializer<std::pair<K, V>> {
        static constexpr bool serializable = true;
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

    template <typename T>
    requires Serializer<T>::serializable
    struct Serializer<std::vector<T>> {
        static constexpr bool serializable = true;
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

    template <typename T>
    requires Serializer<T>::serializable
    struct Serializer<std::unordered_set<T>> {
        static constexpr bool serializable = true;
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

    template <typename K, typename V>
    requires Serializer<K>::serializable && Serializer<V>::serializable
    struct Serializer<std::unordered_map<K, V>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId
            = Common::HashUtils::StrCrc32("std::unordered_map")
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
}
