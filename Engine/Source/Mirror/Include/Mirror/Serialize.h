//
// Created by johnk on 2023/4/20.
//

#pragma once

#include <fstream>
#include <functional>

#include <Mirror/Api.h>

namespace Mirror {
    class Variable;
    class MemberVariable;
    class Class;

    class MIRROR_API SerializeStream {
    public:
        virtual ~SerializeStream() = default;
        virtual void Write(const void* data, size_t size) = 0;
    };

    class MIRROR_API DeserializeStream {
    public:
        virtual ~DeserializeStream() = default;
        virtual void Read(void* data, size_t size) = 0;
        virtual void SeekForward(int32_t offset) = 0;
        virtual void SeekBack(int32_t offset) = 0;
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

    struct MIRROR_API ClassSerializeHeader {
        uint32_t nameSize;
        uint32_t variableNum;
    };

    struct MIRROR_API VariableSerializeHeader {
        uint32_t nameSize;
        uint32_t typeNameSize;
        uint32_t memorySize;
    };

    using VariableSerializer = std::function<void(FileSerializeStream&, const Variable&)>;
    using MemberVariableSerializer = std::function<void(FileSerializeStream&, const MemberVariable&, Any* obj)>;
    using ClassSerializer = std::function<void(FileSerializeStream&, const Class& clazz, Any* obj)>;
    using VariableDeserializer = std::function<void(FileDeserializeStream&, const Variable&)>;
    using MemberVariableDeserializer = std::function<void(FileDeserializeStream&, const MemberVariable&, Any* obj)>;

    using CustomVariableSerializer = std::function<void(FileSerializeStream&, const Variable&, VariableSerializer)>;
    using CustomMemberVariableSerializer = std::function<void(FileSerializeStream&, const MemberVariable&, Any* obj, MemberVariableSerializer)>;
    using CustomClassSerializer = std::function<void(FileSerializeStream&, const Class& clazz, Any* obj, ClassSerializer)>;
    using CustomVariableDeserializer = std::function<void(FileDeserializeStream&, const Variable&, VariableDeserializer)>;
    using CustomMemberVariableDeserializer = std::function<void(FileDeserializeStream&, const MemberVariable&, Any* obj, MemberVariableDeserializer)>;

    template <typename T>
    struct InternalVariableSerializer {
        static VariableSerializer value;
    };

    template <typename T>
    struct InternalVariableDeserializer {
        static VariableDeserializer value;
    };

    template <typename C, typename T>
    struct InternalMemberVariableSerializer {
        static MemberVariableSerializer value;
    };

    template <typename C, typename T>
    struct InternalMemberVariableDeserializer {
        static MemberVariableDeserializer value;
    };
}
