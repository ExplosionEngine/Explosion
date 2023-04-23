//
// Created by johnk on 2023/4/23.
//

#include <gtest/gtest.h>

#include <Common/Path.h>
#include <Mirror/Registry.h>
#include <Mirror/Type.h>
#include <Mirror/Serialization.h>

int ga;
float gb;
std::string gc;

struct TestStruct0 {
    int a;
    float b;
    std::string c;
};

struct MirrorInfoRegistry {
    MirrorInfoRegistry()
    {
        // TODO
    }
};
static MirrorInfoRegistry registry;

TEST(SerializationTest, FileStreamTest)
{
    static std::string fileName = "TestGenerated/SerializationTest.FileStreamTest.bin";
    Common::PathUtils::MakeDirectoriesForFile(fileName);
    {
        std::ofstream file(fileName, std::ios::binary);

        int a = 1;
        float b = 2.0f;
        std::string c = "str";
        size_t cSize = c.size();

        Mirror::FileSerializeStream stream(file);
        stream.Write(&a, sizeof(int));
        stream.Write(&b, sizeof(float));
        stream.Write(&cSize, sizeof(size_t));
        stream.Write(c.data(), c.size());

        file.close();
    }

    {
        std::ifstream file(fileName, std::ios::binary);

        int a;
        float b;
        size_t cSize;
        std::string c;

        Mirror::FileDeserializeStream stream(file);
        stream.SeekForward(sizeof(int));
        stream.Read(&b, sizeof(float));
        stream.SeekBack(sizeof(int) + sizeof(float));
        stream.Read(&a, sizeof(int));
        stream.SeekForward(sizeof(float));
        stream.Read(&cSize, sizeof(size_t));
        c.resize(cSize);
        stream.Read(c.data(), c.size());

        file.close();

        ASSERT_EQ(a, 1);
        ASSERT_EQ(b, 2.0f);
        ASSERT_EQ(c, "str");
    }
}

TEST(SerializationTest, VariableFileSerializationTest)
{
    // TODO
}
