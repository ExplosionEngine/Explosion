//
// Created by johnk on 2023/7/13.
//

#include <filesystem>

#include <Common/Memory.h>
#include <SerializationTest.h>

using namespace Common;

TEST(SerializationTest, FileStreamTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.FileStreamTest.bin";
    std::filesystem::create_directories(fileName.parent_path());

    {
        const uint32_t value = 5; // NOLINT

        BinaryFileSerializeStream stream(fileName.string());
        stream.Seek(3);
        stream.Write(&value, sizeof(uint32_t));
    }

    {
        uint32_t value;

        BinaryFileDeserializeStream stream(fileName.string());
        stream.Seek(3);
        stream.Read(&value, sizeof(uint32_t));
        ASSERT_EQ(value, 5);
    }
}

TEST(SerializationTest, ByteStreamTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Common/SerializationTest.ByteStreamTest.bin";
    std::filesystem::create_directories(fileName.parent_path());

    std::vector<uint8_t> memory;
    {
        const uint32_t value = 5; // NOLINT
        MemorySerializeStream stream(memory);
        stream.Seek(3);
        stream.Write(&value, sizeof(uint32_t));
    }

    {
        uint32_t value;
        MemoryDeserializeStream stream(memory);
        stream.Seek(3);
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
    PerformTypedSerializationTest<std::wstring>(L"hello");
    PerformTypedSerializationTest<std::optional<int>>({});
    PerformTypedSerializationTest<std::optional<int>>(15);
    PerformTypedSerializationTest<std::pair<int, bool>>({ 1, false });
    PerformTypedSerializationTest<std::array<int, 3>>({ 1, 2, 3 });
    PerformTypedSerializationTest<std::vector<int>>({ 1, 2, 3 });
    PerformTypedSerializationTest<std::list<int>>({ 1, 2, 3 });
    PerformTypedSerializationTest<std::unordered_set<int>>({ 1, 2, 3 });
    PerformTypedSerializationTest<std::set<int>>({ 1, 2, 3 });
    PerformTypedSerializationTest<std::unordered_map<int, bool>>({ { 1, false }, { 2, true } });
    PerformTypedSerializationTest<std::map<int, bool>>({ { 1, false }, { 2, true } });
    PerformTypedSerializationTest<std::tuple<int, bool, int>>({ 1, true, 2 });
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
    PerformJsonSerializationTest<std::wstring>(L"hello", R"("hello")");
    PerformJsonSerializationTest<std::optional<int>>({}, "null");
    PerformJsonSerializationTest<std::optional<int>>(15, "15");
    PerformJsonSerializationTest<std::pair<int, bool>>({ 1, false }, R"({"key":1,"value":false})");
    PerformJsonSerializationTest<std::array<int, 3>>({ 1, 2, 3 }, "[1,2,3]");
    PerformJsonSerializationTest<std::vector<int>>({ 1, 2, 3 }, "[1,2,3]");
    PerformJsonSerializationTest<std::list<int>>({ 1, 2, 3 }, "[1,2,3]");
    PerformJsonSerializationTest<std::unordered_set<int>>({ 1, 2, 3 }, "");
    PerformJsonSerializationTest<std::set<int>>({ 1, 2, 3 }, "[1,2,3]");
    PerformJsonSerializationTest<std::unordered_map<int, bool>>({ { 1, false }, { 2, true } }, "");
    PerformJsonSerializationTest<std::map<int, bool>>({ { 1, false }, { 2, true } }, R"([{"key":1,"value":false},{"key":2,"value":true}])");
    PerformJsonSerializationTest<std::tuple<int, bool, int>>({ 1, true, 2 }, R"({"0":1,"1":true,"2":2})");
}
