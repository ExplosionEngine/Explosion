//
// Created by johnk on 2023/7/13.
//

#include <filesystem>

#include <rapidjson/writer.h>

#include <Test/Test.h>
#include <Common/Serialization.h>
#include <Common/Memory.h>

using namespace Common;

template <typename T>
void PerformTypedSerializationTest(const T& inValue, const Test::CustomComparer<T>& inCustomCompareFunc = {})
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.TypedSerializationTest";
    std::filesystem::create_directories(fileName.parent_path());

    {
        BinaryFileSerializeStream stream(fileName.string());
        Serialize<T>(stream, inValue);
    }

    {
        T value;
        BinaryFileDeserializeStream stream(fileName.string());
        Deserialize<T>(stream, value);

        if (inCustomCompareFunc) {
            ASSERT_TRUE(inCustomCompareFunc(inValue, value));
        } else {
            ASSERT_EQ(inValue, value);
        }
    }
}

template <typename T>
void PerformJsonSerializationTest(const T& inValue, const std::string& inExceptJson, const Test::CustomComparer<T>& inCustomCompareFunc = {})
{
    {
        rapidjson::Document document;

        rapidjson::Value jsonValue;
        JsonSerialize<T>(jsonValue, document.GetAllocator(), inValue);
        document.CopyFrom(jsonValue, document.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer writer(buffer);
        document.Accept(writer);

        const auto json = std::string(buffer.GetString(), buffer.GetSize());
        ASSERT_EQ(json, inExceptJson);
    }

    {
        rapidjson::Document document;
        document.Parse(inExceptJson.c_str());

        rapidjson::Value jsonValue;
        jsonValue.CopyFrom(document, document.GetAllocator());

        T value;
        JsonDeserialize<T>(jsonValue, value);
        if (inCustomCompareFunc) {
            ASSERT_TRUE(inCustomCompareFunc(inValue, value));
        } else {
            ASSERT_EQ(inValue, value);
        }
    }
}

TEST(SerializationTest, FileStreamTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.FileStreamTest.bin";
    std::filesystem::create_directories(fileName.parent_path());

    {
        const uint32_t value = 5; // NOLINT

        BinaryFileSerializeStream stream(fileName.string());
        stream.Write(&value, sizeof(uint32_t));
    }

    {
        uint32_t value;

        BinaryFileDeserializeStream stream(fileName.string());
        stream.Read(&value, sizeof(uint32_t));
        ASSERT_EQ(value, 5);
    }
}

TEST(SerializationTest, TypedSerializationTest)
{
    PerformTypedSerializationTest<bool>(false);
    PerformTypedSerializationTest<bool>(true);
    PerformTypedSerializationTest<int8_t>(-1);
    PerformTypedSerializationTest<uint8_t>(1);
    PerformTypedSerializationTest<int16_t>(-2);
    PerformTypedSerializationTest<uint16_t>(2);
    PerformTypedSerializationTest<int32_t>(-3);
    PerformTypedSerializationTest<uint32_t>(3);
    PerformTypedSerializationTest<int64_t>(-4);
    PerformTypedSerializationTest<uint64_t>(4);
    PerformTypedSerializationTest<float>(5.0f);
    PerformTypedSerializationTest<double>(6.0);
    PerformTypedSerializationTest<std::string>("hello");
    PerformTypedSerializationTest<std::optional<int>>({});
    PerformTypedSerializationTest<std::optional<int>>(15);
    PerformTypedSerializationTest<std::pair<int, bool>>({ 1, false });
    PerformTypedSerializationTest<std::vector<int>>({ 1, 2, 3 }, ::Test::CompareVec<int>);
    PerformTypedSerializationTest<std::unordered_set<int>>({ 1, 2, 3 }, ::Test::CompareUnorderedSet<int>);
    PerformTypedSerializationTest<std::unordered_map<int, bool>>({ { 1, false }, { 2, true } }, ::Test::CompareUnorderedMap<int, bool>);
}

TEST(SerializationTest, JsonSerializeTest)
{
    PerformJsonSerializationTest<bool>(false, "false");
    PerformJsonSerializationTest<bool>(true, "true");
    PerformJsonSerializationTest<int8_t>(-1, "-1");
    PerformJsonSerializationTest<uint8_t>(1, "1");
    PerformJsonSerializationTest<int16_t>(-2, "-2");
    PerformJsonSerializationTest<uint16_t>(2, "2");
    PerformJsonSerializationTest<int32_t>(-3, "-3");
    PerformJsonSerializationTest<uint32_t>(3, "3");
    PerformJsonSerializationTest<int64_t>(-4, "-4");
    PerformJsonSerializationTest<uint64_t>(4, "4");
    PerformJsonSerializationTest<float>(5.0f, "5.0");
    PerformJsonSerializationTest<double>(6.0, "6.0");
    PerformJsonSerializationTest<std::string>("hello", R"("hello")");
    PerformJsonSerializationTest<std::optional<int>>({}, "null");
    PerformJsonSerializationTest<std::optional<int>>(15, "15");
    PerformJsonSerializationTest<std::pair<int, bool>>({ 1, false }, R"({"key":1,"value":false})");
    PerformJsonSerializationTest<std::vector<int>>({ 1, 2, 3 }, "[1,2,3]", ::Test::CompareVec<int>);
    PerformJsonSerializationTest<std::unordered_set<int>>({ 1, 2, 3 }, "[1,2,3]", ::Test::CompareUnorderedSet<int>);
    PerformJsonSerializationTest<std::unordered_map<int, bool>>({ { 1, false }, { 2, true } }, R"([{"key":1,"value":false},{"key":2,"value":true}])", ::Test::CompareUnorderedMap<int, bool>);
}
