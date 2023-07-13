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

namespace Common {
    class SerializeStream {
    public:
        NON_COPYABLE(SerializeStream)
        virtual ~SerializeStream();

        virtual void Write(const void* data, size_t size) = 0;

    protected:
        SerializeStream();
    };

    class DeserializeStream {
    public:
        NON_COPYABLE(DeserializeStream);
        virtual ~DeserializeStream();

        virtual void Read(void* data, size_t size) = 0;

    protected:
        DeserializeStream();
    };

    class BinaryFileSerializeStream : public SerializeStream {
    public:
        NON_COPYABLE(BinaryFileSerializeStream)
        explicit BinaryFileSerializeStream(const std::string& inFileName);
        ~BinaryFileSerializeStream() override;
        void Write(const void* data, size_t size) override;

    private:
        std::ofstream file;
    };

    class BinaryFileDeserializeStream : public DeserializeStream {
    public:
        NON_COPYABLE(BinaryFileDeserializeStream)
        explicit BinaryFileDeserializeStream(const std::string& inFileName);
        ~BinaryFileDeserializeStream() override;
        void Read(void* data, size_t size) override;

    private:
        std::ifstream file;
    };

    template <typename T>
    struct Serializer {
        static constexpr bool serializable = false;

        static void Serialize(SerializeStream& stream, const T& value)
        {
            Unimplement();
        }

        static void Deserialize(DeserializeStream& stream, T& value)
        {
            Unimplement();
        }
    };
}

namespace Common {
    template <>
    struct Serializer<bool> {
        static constexpr bool serializable = true;

        static void Serialize(SerializeStream& stream, const bool& value)
        {
            stream.Write(&value, sizeof(bool));
        }

        static void Deserialize(DeserializeStream& stream, bool& value)
        {
            stream.Read(&value, sizeof(bool));
        }
    };

    template <typename T>
    requires std::is_arithmetic_v<T>
    struct Serializer<T> {
        static constexpr bool serializable = true;

        static void Serialize(SerializeStream& stream, const T& value)
        {
            stream.Write(&value, sizeof(T));
        }

        static void Deserialize(DeserializeStream& stream, T& value)
        {
            stream.Read(&value, sizeof(T));
        }
    };

    template <>
    struct Serializer<std::string> {
        static constexpr bool serializable = true;

        static void Serialize(SerializeStream& stream, const std::string& value)
        {
            size_t size = value.size();
            Serializer<size_t>::Serialize(stream, size);
            stream.Write(value.data(), value.size());
        }

        static void Deserialize(DeserializeStream& stream, std::string& value)
        {
            size_t size;
            Serializer<size_t>::Deserialize(stream, size);
            value.resize(size);
            stream.Read(value.data(), size);
        }
    };

    template <typename T>
    requires Serializer<T>::serializable
    struct Serializer<std::optional<T>> {
        static constexpr bool serializable = true;

        static void Serialize(SerializeStream& stream, const std::optional<T>& value)
        {
            bool hasValue = value.has_value();
            Serializer<bool>::Serialize(stream, hasValue);

            if (hasValue) {
                Serializer<T>::Serialize(stream, value.value());
            }
        }

        static void Deserialize(DeserializeStream& stream, std::optional<T>& value)
        {
            value.reset();
            bool hasValue;
            Serializer<bool>::Deserialize(stream, hasValue);

            if (hasValue) {
                T temp;
                Serializer<T>::Deserialize(stream, temp);
                value.emplace(std::move(temp));
            }
        }
    };

    template <typename K, typename V>
    requires Serializer<K>::serializable && Serializer<V>::serializable
    struct Serializer<std::pair<K, V>> {
        static constexpr bool serializable = true;

        static void Serialize(SerializeStream& stream, const std::pair<K, V>& value)
        {
            Serializer<K>::Serialize(stream, value.first);
            Serializer<V>::Serialize(stream, value.second);
        }

        static void Deserialize(DeserializeStream& stream, std::pair<K, V>& value)
        {
            Serializer<K>::Deserialize(stream, value.first);
            Serializer<V>::Deserialize(stream, value.second);
        }
    };

    template <typename T>
    requires Serializer<T>::serializable
    struct Serializer<std::vector<T>> {
        static constexpr bool serializable = true;

        static void Serialize(SerializeStream& stream, const std::vector<T>& value)
        {
            size_t size = value.size();
            Serializer<size_t>::Serialize(stream, size);

            for (auto i = 0; i < size; i++) {
                Serializer<T>::Serialize(stream, value[i]);
            }
        }

        static void Deserialize(DeserializeStream& stream, std::vector<T>& value)
        {
            value.clear();

            size_t size;
            Serializer<size_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                T element;
                Serializer<T>::Deserialize(stream, element);
                value.emplace_back(std::move(element));
            }
        }
    };

    template <typename T>
    requires Serializer<T>::serializable
    struct Serializer<std::unordered_set<T>> {
        static constexpr bool serializable = true;

        static void Serialize(SerializeStream& stream, const std::unordered_set<T>& value)
        {
            size_t size = value.size();
            Serializer<size_t>::Serialize(stream, size);

            for (const auto& element : value) {
                Serializer<T>::Serialize(stream, element);
            }
        }

        static void Deserialize(DeserializeStream& stream, std::unordered_set<T>& value)
        {
            value.clear();

            size_t size;
            Serializer<size_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                T temp;
                Serializer<T>::Deserialize(stream, temp);
                value.emplace(std::move(temp));
            }
        }
    };

    template <typename K, typename V>
    requires Serializer<K>::serializable && Serializer<V>::serializable
    struct Serializer<std::unordered_map<K, V>> {
        static constexpr bool serializable = true;

        static void Serialize(SerializeStream& stream, const std::unordered_map<K, V>& value)
        {
            size_t size = value.size();
            Serializer<size_t>::Serialize(stream, size);

            for (const auto& pair : value) {
                Serializer<K>::Serialize(stream, pair.first);
                Serializer<V>::Serialize(stream, pair.second);
            }
        }

        static void Deserialize(DeserializeStream& stream, std::unordered_map<K, V>& value)
        {
            value.clear();

            size_t size;
            Serializer<size_t>::Deserialize(stream, size);

            value.reserve(size);
            for (auto i = 0; i < size; i++) {
                std::pair<K, V> pair;
                Serializer<K>::Deserialize(stream, pair.first);
                Serializer<V>::Deserialize(stream, pair.second);
                value.emplace(std::move(pair));
            }
        }
    };
}
