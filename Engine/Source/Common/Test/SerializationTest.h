//
// Created by johnk on 2024/9/8.
//

#pragma once

#include <filesystem>

#include <rapidjson/writer.h>

#include <Test/Test.h>
#include <Common/Serialization.h>
#include <Common/Memory.h>
#include <Common/IO.h>

inline std::string FltToJson(float value)
{
    rapidjson::Document document;
    document.SetFloat(value);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    document.Accept(writer);
    return std::string(buffer.GetString(), buffer.GetSize()); // NOLINT
}

inline std::string GetEndianString(std::endian e)
{
    static std::unordered_map<std::endian, std::string> map = {
        { std::endian::big, "std::endian::big" },
        { std::endian::little, "std::endian::little" },
        { std::endian::native, "std::endian::native" }
    };
    return map.at(e);
}

template <typename T>
void PerformTypedSerializationTestWithStream(
    const std::string& contextString,
    const std::function<Common::UniqueRef<Common::BinarySerializeStream>()>& createSerializeStream,
    const std::function<Common::UniqueRef<Common::BinaryDeserializeStream>()>& createDeserializeStream,
    const T& inValue)
{
    {
        const auto stream = createSerializeStream();
        Common::Serialize<T>(*stream, inValue);
    }

    {
        T value;
        const auto stream = createDeserializeStream();
        Common::Deserialize<T>(*stream, value);

        std::cout << "PerformTypedSerializationTestWithStream, context: " << contextString << std::endl;
        ASSERT_EQ(inValue, value);
    }
}

template <typename T, std::endian E>
void PerformTypedSerializationTestWithEndian(const T& inValue)
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.TypedSerializationTest";
    std::filesystem::create_directories(fileName.parent_path());

    PerformTypedSerializationTestWithStream<T>(
        std::format("BinaryFile<{}, {}>", typeid(T).name(), GetEndianString(E)),
        []() -> Common::UniqueRef<Common::BinarySerializeStream> { return { new Common::BinaryFileSerializeStream<E>(fileName.string()) }; },
        []() -> Common::UniqueRef<Common::BinaryDeserializeStream> { return { new Common::BinaryFileDeserializeStream<E>(fileName.string()) }; },
        inValue);

    std::vector<uint8_t> buffer;
    PerformTypedSerializationTestWithStream<T>(
        std::format("Memory<{}, {}>", typeid(T).name(), GetEndianString(E)),
        [&]() -> Common::UniqueRef<Common::BinarySerializeStream> { return { new Common::MemorySerializeStream<E>(buffer) }; },
        [&]() -> Common::UniqueRef<Common::BinaryDeserializeStream> { return { new Common::MemoryDeserializeStream<E>(buffer) }; },
        inValue);
}

template <typename T>
void PerformTypedSerializationTest(const T& inValue)
{
    PerformTypedSerializationTestWithEndian<T, std::endian::little>(inValue);
    PerformTypedSerializationTestWithEndian<T, std::endian::big>(inValue);
    PerformTypedSerializationTestWithEndian<T, std::endian::native>(inValue);
}

template <typename T>
void PerformJsonSerializationTest(const T& inValue, const std::string& inExceptJson)
{
    std::string json;
    {
        rapidjson::Document document;

        rapidjson::Value jsonValue;
        Common::JsonSerialize<T>(jsonValue, document.GetAllocator(), inValue);
        document.CopyFrom(jsonValue, document.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer writer(buffer);
        document.Accept(writer);

        json = std::string(buffer.GetString(), buffer.GetSize());
        if (!inExceptJson.empty()) {
            ASSERT_EQ(json, inExceptJson);
        }
    }

    {
        rapidjson::Document document;
        document.Parse(json.c_str());

        rapidjson::Value jsonValue;
        jsonValue.CopyFrom(document, document.GetAllocator());

        T value;
        Common::JsonDeserialize<T>(jsonValue, value);
        ASSERT_EQ(inValue, value);
    }
}
