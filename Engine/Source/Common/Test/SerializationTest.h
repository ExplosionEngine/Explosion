//
// Created by johnk on 2024/9/8.
//

#pragma once

#include <filesystem>

#include <rapidjson/writer.h>

#include <Test/Test.h>
#include <Common/Serialization.h>
#include <Common/Memory.h>

inline std::string FltToJson(float value)
{
    rapidjson::Document document;
    document.SetFloat(value);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    document.Accept(writer);
    return std::string(buffer.GetString(), buffer.GetSize()); // NOLINT
}

template <typename T>
void PerformTypedSerializationTestWithStream(
    const std::function<Common::UniqueRef<BinarySerializeStream>()>& createSerializeStream,
    const std::function<Common::UniqueRef<BinaryDeserializeStream>()>& createDeserializeStream,
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

        ASSERT_EQ(inValue, value);
    }
}

template <typename T, std::endian E>
void PerformTypedSerializationTestWithEndian(const T& inValue)
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.TypedSerializationTest";
    std::filesystem::create_directories(fileName.parent_path());

    PerformTypedSerializationTestWithStream<T>(
        []() -> Common::UniqueRef<BinarySerializeStream> { return { new BinaryFileSerializeStream<E>(fileName.string()) }; },
        []() -> Common::UniqueRef<BinaryDeserializeStream> { return { new BinaryFileDeserializeStream<E>(fileName.string()) }; },
        inValue);

    std::vector<uint8_t> buffer;
    PerformTypedSerializationTestWithStream<T>(
        [&]() -> Common::UniqueRef<BinarySerializeStream> { return { new MemorySerializeStream<E>(buffer) }; },
        [&]() -> Common::UniqueRef<BinaryDeserializeStream> { return { new MemoryDeserializeStream<E>(buffer) }; },
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
