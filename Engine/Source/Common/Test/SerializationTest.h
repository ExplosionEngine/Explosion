//
// Created by johnk on 2024/9/8.
//

#pragma once

#include <rapidjson/writer.h>

#include <Test/Test.h>
#include <Common/Serialization.h>
#include <Common/Memory.h>
#include <Common/FileSystem.h>

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
    const std::function<Common::UniquePtr<Common::BinarySerializeStream>()>& createSerializeStream,
    const std::function<Common::UniquePtr<Common::BinaryDeserializeStream>()>& createDeserializeStream,
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
    static Common::Path fileName = "../Test/Generated/Common/SerializationTest.TypedSerializationTest";

    PerformTypedSerializationTestWithStream<T>(
        []() -> Common::UniquePtr<Common::BinarySerializeStream> { return {new Common::BinaryFileSerializeStream<E>(fileName.String()) }; },
        []() -> Common::UniquePtr<Common::BinaryDeserializeStream> { return {new Common::BinaryFileDeserializeStream<E>(fileName.String()) }; },
        inValue);

    std::vector<uint8_t> buffer;
    PerformTypedSerializationTestWithStream<T>(
        [&]() -> Common::UniquePtr<Common::BinarySerializeStream> { return {new Common::MemorySerializeStream<E>(buffer) }; },
        [&]() -> Common::UniquePtr<Common::BinaryDeserializeStream> { return {new Common::MemoryDeserializeStream<E>(buffer) }; },
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
void PerformTypeSerializationWithFileTest(const std::string& inFile, const T& inValue)
{
    Common::SerializeToFile<T>(inFile, inValue);

    T value;
    Common::DeserializeFromFile(inFile, value);
    ASSERT_EQ(inValue, value);
}

template <typename T>
void PerformJsonSerializationTest(const T& inValue, const std::string& inExceptJson)
{
    std::string json;
    {
        rapidjson::Document document;
        Common::JsonSerialize<T>(document, document.GetAllocator(), inValue);

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

        T value;
        Common::JsonDeserialize<T>(document, value);
        ASSERT_EQ(inValue, value);
    }
}

template <typename T>
void PerformJsonSerializationWithFileTest(const std::string& inFile, const T& inValue)
{
    Common::JsonSerializeToFile<T>(inFile, inValue);

    T value;
    Common::JsonDeserializeFromFile(inFile, value);
    ASSERT_EQ(inValue, value);
}
