//
// Created by johnk on 2023/7/13.
//

#include <filesystem>

#include <Common/Memory.h>
#include <SerializationTest.h>

using namespace Common;

TEST(SerializationTest, FileStreamTest)
{
    static Common::Path fileName = "../Test/Generated/Common/SerializationTest.FileStreamTest.bin";
    {
        const uint32_t value = 5; // NOLINT

        BinaryFileSerializeStream stream(fileName.String());
        stream.Seek(3);
        stream.Write<uint32_t>(value);
    }

    {
        uint32_t value;

        BinaryFileDeserializeStream stream(fileName.String());
        stream.Seek(3);
        stream.Read<uint32_t>(value);
        ASSERT_EQ(value, 5);
    }
}

TEST(SerializationTest, ByteStreamTest)
{
    std::vector<uint8_t> memory;
    {
        const uint32_t value = 5; // NOLINT
        MemorySerializeStream stream(memory);
        stream.Seek(3);
        stream.Write<uint32_t>(value);
    }

    {
        uint32_t value;
        MemoryDeserializeStream stream(memory);
        stream.Seek(3);
        stream.Read<uint32_t>(value);
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

TEST(SerializationTest, TypedSerializationWithFileTest)
{
    static std::string fileName = "../Test/Generated/Common/SerializationTest.TypedSerializationWithFileTest.bin";

    PerformTypeSerializationWithFileTest<bool>(fileName, false);
    PerformTypeSerializationWithFileTest<bool>(fileName,true);
    PerformTypeSerializationWithFileTest<int8_t>(fileName, -1);
    PerformTypeSerializationWithFileTest<uint8_t>(fileName, 1);
    PerformTypeSerializationWithFileTest<int16_t>(fileName, -2);
    PerformTypeSerializationWithFileTest<uint16_t>(fileName, 2);
    PerformTypeSerializationWithFileTest<int32_t>(fileName, -3);
    PerformTypeSerializationWithFileTest<uint32_t>(fileName, 3);
    PerformTypeSerializationWithFileTest<int64_t>(fileName, -4);
    PerformTypeSerializationWithFileTest<uint64_t>(fileName, 4);
    PerformTypeSerializationWithFileTest<float>(fileName, 5.0f);
    PerformTypeSerializationWithFileTest<double>(fileName, 6.0);
    PerformTypeSerializationWithFileTest<std::string>(fileName, "hello");
    PerformTypeSerializationWithFileTest<std::wstring>(fileName, L"hello");
    PerformTypeSerializationWithFileTest<std::optional<int>>(fileName, {});
    PerformTypeSerializationWithFileTest<std::optional<int>>(fileName, 15);
    PerformTypeSerializationWithFileTest<std::pair<int, bool>>(fileName, { 1, false });
    PerformTypeSerializationWithFileTest<std::array<int, 3>>(fileName, { 1, 2, 3 });
    PerformTypeSerializationWithFileTest<std::vector<int>>(fileName, { 1, 2, 3 });
    PerformTypeSerializationWithFileTest<std::list<int>>(fileName, { 1, 2, 3 });
    PerformTypeSerializationWithFileTest<std::unordered_set<int>>(fileName, { 1, 2, 3 });
    PerformTypeSerializationWithFileTest<std::set<int>>(fileName, { 1, 2, 3 });
    PerformTypeSerializationWithFileTest<std::unordered_map<int, bool>>(fileName, { { 1, false }, { 2, true } });
    PerformTypeSerializationWithFileTest<std::map<int, bool>>(fileName, { { 1, false }, { 2, true } });
    PerformTypeSerializationWithFileTest<std::tuple<int, bool, int>>(fileName, { 1, true, 2 });
}

TEST(SerializationTest, JsonSerializationTest)
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
    PerformJsonSerializationTest<std::map<std::string, int>>({ { "1", 1 }, { "2", 2 } }, R"([{"key":"1","value":1},{"key":"2","value":2}])");
    PerformJsonSerializationTest<std::tuple<int, bool, int>>({ 1, true, 2 }, R"({"0":1,"1":true,"2":2})");
}

TEST(SerializationTest, JsonSerializationWithFileTest)
{
    static std::string fileName = "../Test/Generated/Common/SerializationTest.JsonSerializationWithFileTest.bin";

    PerformJsonSerializationWithFileTest<bool>(fileName, false);
    PerformJsonSerializationWithFileTest<bool>(fileName, true);
    PerformJsonSerializationWithFileTest<int8_t>(fileName, -1);
    PerformJsonSerializationWithFileTest<uint8_t>(fileName, 1);
    PerformJsonSerializationWithFileTest<int16_t>(fileName, -2);
    PerformJsonSerializationWithFileTest<uint16_t>(fileName, 2);
    PerformJsonSerializationWithFileTest<int32_t>(fileName, -3);
    PerformJsonSerializationWithFileTest<uint32_t>(fileName, 3);
    PerformJsonSerializationWithFileTest<int64_t>(fileName, -4);
    PerformJsonSerializationWithFileTest<uint64_t>(fileName, 4);
    PerformJsonSerializationWithFileTest<float>(fileName, 5.0f);
    PerformJsonSerializationWithFileTest<double>(fileName, 6.0);
    PerformJsonSerializationWithFileTest<std::string>(fileName, "hello");
    PerformJsonSerializationWithFileTest<std::wstring>(fileName, L"hello");
    PerformJsonSerializationWithFileTest<std::optional<int>>(fileName, {});
    PerformJsonSerializationWithFileTest<std::optional<int>>(fileName, 15);
    PerformJsonSerializationWithFileTest<std::pair<int, bool>>(fileName, { 1, false });
    PerformJsonSerializationWithFileTest<std::array<int, 3>>(fileName, { 1, 2, 3 });
    PerformJsonSerializationWithFileTest<std::vector<int>>(fileName, { 1, 2, 3 });
    PerformJsonSerializationWithFileTest<std::list<int>>(fileName, { 1, 2, 3 });
    PerformJsonSerializationWithFileTest<std::unordered_set<int>>(fileName, { 1, 2, 3 });
    PerformJsonSerializationWithFileTest<std::set<int>>(fileName, { 1, 2, 3 });
    PerformJsonSerializationWithFileTest<std::unordered_map<int, bool>>(fileName, { { 1, false }, { 2, true } });
    PerformJsonSerializationWithFileTest<std::map<int, bool>>(fileName, { { 1, false }, { 2, true } });
    PerformJsonSerializationWithFileTest<std::map<std::string, int>>(fileName, { { "1", 1 }, { "2", 2 } });
    PerformJsonSerializationWithFileTest<std::tuple<int, bool, int>>(fileName, { 1, true, 2 });
}
