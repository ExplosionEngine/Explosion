//
// Created by johnk on 2023/7/13.
//

#include <filesystem>

#include <Test/Test.h>

#include <Common/Serialization.h>

using namespace Common;

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

TEST(SerializationTest, SerializationTest)
{
    // TODO
}

TEST(SerializationTest, JsonSerializeTest)
{
    // TODO
}
