//
// Created by johnk on 2024/9/8.
//

#pragma once

#include <filesystem>

#include <rapidjson/writer.h>

#include <Test/Test.h>
#include <Common/Serialization.h>

template <typename T, std::endian E>
void PerformTypedSerializationTestWithEndian(const T& inValue)
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.TypedSerializationTest";
    std::filesystem::create_directories(fileName.parent_path());

    {
        Common::BinaryFileSerializeStream<E> stream(fileName.string());
        Common::Serialize<T>(stream, inValue);
    }

    {
        T value;
        Common::BinaryFileDeserializeStream<E> stream(fileName.string());
        Common::Deserialize<T>(stream, value);

        ASSERT_EQ(inValue, value);
    }
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
