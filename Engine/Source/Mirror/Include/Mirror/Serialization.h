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

        // TODO container
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

        // TODO container
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

    // TODO math
    // TODO std container
}
