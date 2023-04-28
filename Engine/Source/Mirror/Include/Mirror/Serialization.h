//
// Created by johnk on 2023/4/20.
//

#pragma once

#include <fstream>
#include <functional>
#include <cstdint>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>

#include <Mirror/Api.h>
#include <Mirror/Type.h>

namespace Mirror {
    template <typename T>
    struct TypeSerializationSupport {
        static constexpr bool value = false;
    };

    template <typename T>
    requires std::is_arithmetic_v<T>
    struct TypeSerializationSupport<T> {
        static constexpr bool value = true;
    };

    template <>
    struct TypeSerializationSupport<std::string> {
        static constexpr bool value = true;
    };

    template <typename T>
    requires TypeSerializationSupport<T>::value
    struct TypeSerializationSupport<std::vector<T>> {
        static constexpr bool value = true;
    };

    template <typename T>
    requires TypeSerializationSupport<T>::value
    struct TypeSerializationSupport<std::unordered_set<T>> {
        static constexpr bool value = true;
    };

    template <typename K, typename V>
    requires TypeSerializationSupport<K>::value && TypeSerializationSupport<V>::value
    struct TypeSerializationSupport<std::unordered_map<K, V>> {
        static constexpr bool value = true;
    };
}

namespace Mirror {
    class MIRROR_API SerializeStream {
    public:
        virtual ~SerializeStream() = default;
        virtual void Write(const void* data, size_t size) = 0;

        template <typename T>
        requires std::is_arithmetic_v<T>
        SerializeStream& operator<<(T value)
        {
            T tValue = value;
            Write(&tValue, sizeof(T));
            return *this;
        }

        SerializeStream& operator<<(const std::string& string)
        {
            size_t size = string.size();
            Write(&size, sizeof(size_t));
            Write(string.data(), string.size());
            return *this;
        }

        template <typename T>
        requires TypeSerializationSupport<T>::value
        SerializeStream& operator<<(const std::vector<T>& vector)
        {
            size_t size = vector.size();
            Write(&size, sizeof(size_t));
            for (auto i = 0; i < size; i++) {
                this->operator<<(vector[i]);
            }
            return *this;
        }

        template <typename T>
        requires TypeSerializationSupport<T>::value
        SerializeStream& operator<<(const std::unordered_set<T>& set)
        {
            size_t size = set.size();
            Write(&size, sizeof(size_t));
            for (const auto& item : set) {
                this->operator<<(item);
            }
            return *this;
        }

        template <typename K, typename V>
        requires TypeSerializationSupport<K>::value && TypeSerializationSupport<V>::value
        SerializeStream& operator<<(const std::unordered_map<K, V>& map)
        {
            size_t size = map.size();
            Write(&size, sizeof(size_t));
            for (const auto& iter : map) {
                this->operator<<(iter.first);
                this->operator<<(iter.second);
            }
            return *this;
        }

        // TODO math
    };

    class MIRROR_API DeserializeStream {
    public:
        virtual ~DeserializeStream() = default;
        virtual void Read(void* data, size_t size) = 0;
        virtual void SeekForward(int32_t offset) = 0;
        virtual void SeekBack(int32_t offset) = 0;

        template <typename T>
        requires std::is_arithmetic_v<T>
        DeserializeStream& operator>>(T& value)
        {
            Read(&value, sizeof(T));
            return *this;
        }

        DeserializeStream& operator>>(std::string& string)
        {
            size_t size;
            Read(&size, sizeof(size_t));

            string.resize(size);
            Read(string.data(), string.size());
            return *this;
        }

        template <typename T>
        requires TypeSerializationSupport<T>::value
        DeserializeStream& operator>>(std::vector<T>& vector)
        {
            size_t size;
            Read(&size, sizeof(size_t));

            vector.reserve(size);
            for (auto i = 0; i < size; i++) {
                T value;
                this->operator>>(value);
                vector.emplace_back(std::move(value));
            }
            return *this;
        }

        template <typename T>
        requires TypeSerializationSupport<T>::value
        DeserializeStream& operator>>(std::unordered_set<T>& set)
        {
            size_t size;
            Read(&size, sizeof(size_t));

            set.reserve(size);
            for (auto i = 0; i < size; i++) {
                T value;
                this->operator>>(value);
                set.emplace(std::move(value));
            }
            return *this;
        }

        template <typename K, typename V>
        requires TypeSerializationSupport<K>::value && TypeSerializationSupport<V>::value
        DeserializeStream& operator>>(std::unordered_map<K, V>& map)
        {
            size_t size;
            Read(&size, sizeof(size_t));

            map.reserve(size);
            for (auto i = 0; i < size; i++) {
                K key;
                V value;
                this->operator>>(key);
                this->operator>>(value);
                map.emplace(std::move(key), std::move(value));
            }
            return *this;
        }

        // TODO math
    };

    class MIRROR_API FileSerializeStream : public SerializeStream {
    public:
        explicit FileSerializeStream(std::ofstream& inFile) : file(inFile) {}
        ~FileSerializeStream() override = default;

        void Write(const void* data, size_t size) override
        {
            file.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
        }

    private:
        std::ofstream& file;
    };

    class MIRROR_API FileDeserializeStream : public DeserializeStream {
    public:
        explicit FileDeserializeStream(std::ifstream& inFile) : file(inFile) {}
        ~FileDeserializeStream() override = default;

        void Read(void* data, size_t size) override
        {
            file.read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(size));
        }

        void SeekForward(int32_t offset) override
        {
            file.seekg(offset, std::ios::cur);
        }

        void SeekBack(int32_t offset) override
        {
            file.seekg(-offset, std::ios::cur);
        }

    private:
        std::ifstream& file;
    };

    class MIRROR_API AutoCloseFileSerializeStream : public FileSerializeStream {
    public:
        explicit AutoCloseFileSerializeStream(const std::string& filePath) : file(filePath), FileSerializeStream(file) {}

        ~AutoCloseFileSerializeStream() override
        {
            if (file.is_open()) {
                file.close();
            }
        }

    private:
        std::ofstream file;
    };

    class MIRROR_API AutoCloseFileDeserializeStream : public FileDeserializeStream {
    public:
        explicit AutoCloseFileDeserializeStream(const std::string& filePath) : file(filePath), FileDeserializeStream(file) {}

        ~AutoCloseFileDeserializeStream() override
        {
            if (file.is_open()) {
                file.close();
            }
        }

    private:
        std::ifstream file;
    };
}
