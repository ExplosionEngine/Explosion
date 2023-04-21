//
// Created by johnk on 2023/4/20.
//

#pragma once

#include <fstream>
#include <functional>
#include <cstdint>

#include <Mirror/Api.h>
#include <Mirror/Type.h>

#define MIRROR_IMPL_VARIABLE_SERIALIZER(typeName) template <> VariableSerializer InternalVariableSerializer<typeName>::value = [](FileSerializeStream& stream, const Variable& variable) -> void
#define MIRROR_IMPL_VARIABLE_DESERIALIZER(typeName) template <> VariableDeserializer InternalVariableDeserializer<typeName>::value = [](FileDeserializeStream& stream, const Variable& variable) -> void
#define MIRROR_IMPL_MEMBER_VARIABLE_SERIALIZER(typeName) template <> MemberVariableSerializer InternalMemberVariableSerializer<typeName>::value = [](FileSerializeStream& stream, const MemberVariable& variable, Any* object) -> void
#define MIRROR_IMPL_MEMBER_VARIABLE_DESERIALIZER(typeName) template <> MemberVariableDeserializer InternalMemberVariableDeserializer<typeName>::value = [](FileDeserializeStream& stream, const MemberVariable& variable, Any* object) -> void

#define MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(inTypeName) \
    template <> VariableSerializer InternalVariableSerializer<inTypeName>::value = [](FileSerializeStream& stream, const Variable& variable) -> void { \
        const std::string& name = variable.GetName(); \
        std::string typeName = #inTypeName; \
        auto value = variable.Get().CastTo<inTypeName>(); \
        \
        VariableSerializeHeader header; \
        header.nameSize = name.size(); \
        header.typeNameSize = typeName.size(); \
        header.stride = sizeof(inTypeName); \
        header.arraySize = 1; \
        \
        stream.Write(&header, sizeof(VariableSerializeHeader)); \
        stream.Write(name.data(), name.size()); \
        stream.Write(typeName.data(), typeName.size()); \
        stream.Write(&value, sizeof(inTypeName)); \
    }; \
    \
    template <> VariableDeserializer InternalVariableDeserializer<inTypeName>::value = [](FileDeserializeStream& stream, const Variable& variable) -> void { \
        VariableSerializeHeader header; \
        stream.Read(&header, sizeof(VariableSerializeHeader));\
        Assert(header.arraySize == 1); \
        \
        std::string name; \
        name.resize(header.nameSize); \
        stream.Read(name.data(), name.size()); \
        Assert(name == variable.GetName()); \
        \
        std::string typeName; \
        typeName.resize(header.typeNameSize); \
        stream.Read(typeName.data(), typeName.size()); \
        Assert(typeName == #inTypeName); \
        \
        inTypeName memory; \
        stream.Read(&memory, sizeof(inTypeName)); \
        variable.Set(memory); \
    }; \
    \
    template <> MemberVariableSerializer InternalMemberVariableSerializer<inTypeName>::value = [](FileSerializeStream& stream, const MemberVariable& variable, Any* object) -> void { \
        const std::string& name = variable.GetName(); \
        std::string typeName = #inTypeName; \
        auto value = variable.Get(object).CastTo<inTypeName>(); \
        \
        VariableSerializeHeader header; \
        header.nameSize = name.size(); \
        header.typeNameSize = typeName.size(); \
        header.stride = sizeof(inTypeName); \
        header.arraySize = 1; \
        \
        stream.Write(&header, sizeof(VariableSerializeHeader)); \
        stream.Write(name.data(), name.size()); \
        stream.Write(typeName.data(), typeName.size()); \
        stream.Write(&value, sizeof(inTypeName)); \
    }; \
    template <> MemberVariableDeserializer InternalMemberVariableDeserializer<inTypeName>::value = [](FileDeserializeStream& stream, const MemberVariable& variable, Any* object) -> void { \
        VariableSerializeHeader header; \
        stream.Read(&header, sizeof(VariableSerializeHeader)); \
        Assert(header.arraySize = 1); \
        \
        std::string name; \
        name.resize(header.nameSize); \
        stream.Read(name.data(), name.size()); \
        Assert(name == variable.GetName()); \
        \
        std::string typeName; \
        typeName.resize(header.typeNameSize); \
        stream.Read(typeName.data(), typeName.size()); \
        Assert(typeName == #inTypeName); \
        \
        inTypeName memory; \
        stream.Read(&memory, sizeof(inTypeName)); \
        variable.Set(object, memory); \
    }; \

namespace Mirror {
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
        uint32_t stride;
        uint32_t arraySize;
    };

    template <typename T> struct InternalVariableSerializer { static VariableSerializer value; };
    template <typename T> struct InternalVariableDeserializer { static VariableDeserializer value; };
    template <typename T> struct InternalMemberVariableSerializer { static MemberVariableSerializer value; };
    template <typename T> struct InternalMemberVariableDeserializer { static MemberVariableDeserializer value; };

    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(bool)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(float)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(double)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(int8_t)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(int16_t)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(int32_t)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(int64_t)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(uint8_t)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(uint16_t)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(uint32_t)
    MIRROR_IMPL_BASIC_TYPE_SERIALIZATION_FUNC(uint64_t)
    // TODO math
    // TODO std container
}
