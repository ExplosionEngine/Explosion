//
// Created by johnk on 2023/7/13.
//

#include <gtest/gtest.h>

#include <Common/Path.h>
#include <Common/Serialization.h>

using namespace Common;

TEST(SerializationTest, FileStreamTest)
{
    static std::string fileName = "TestGenerated/SerializationTest.FileStreamTest.bin";
    Common::PathUtils::MakeDirectoriesForFile(fileName);
    {
        uint32_t value = 5;

        BinaryFileSerializeStream stream(fileName);
        stream.Write(&value, sizeof(uint32_t));
    }

    {
        uint32_t value;

        BinaryFileDeserializeStream stream(fileName);
        stream.Read(&value, sizeof(uint32_t));
        ASSERT_EQ(value, 5);
    }
}
