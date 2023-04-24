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
        Mirror::Registry::Get()
            .Global()
                .Variable<&ga>("ga")
                .Variable<&gb>("gb")
                .Variable<&gc>("gc");

        Mirror::Registry::Get()
            .Class<TestStruct0>("TestStruct0")
                .Constructor<>(Mirror::NamePresets::defaultConstructor)
                .MemberVariable<&TestStruct0::a>("a")
                .MemberVariable<&TestStruct0::b>("b")
                .MemberVariable<&TestStruct0::c>("c");
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
    static std::string fileName = "TestGenerated/SerializationTest.VariableFileSerializationTest.bin";
    Common::PathUtils::MakeDirectoriesForFile(fileName);
    {
        std::ofstream file(fileName, std::ios::binary);
        Mirror::FileSerializeStream stream(file);

        ga = 1;
        gb = 2.0f;
        gc = "3";

        const auto& globalScope = Mirror::GlobalScope::Get();
        globalScope.GetVariable("ga").Serialize(stream);
        globalScope.GetVariable("gb").Serialize(stream);
        globalScope.GetVariable("gc").Serialize(stream);

        file.close();
    }

    {
        ga = 4;
        gb = 5.0f;
        gc = "6";

        std::ifstream file(fileName, std::ios::binary);
        Mirror::FileDeserializeStream stream(file);

        const auto& globalScope = Mirror::GlobalScope::Get();
        globalScope.GetVariable("ga").Deserialize(stream);
        globalScope.GetVariable("gb").Deserialize(stream);
        globalScope.GetVariable("gc").Deserialize(stream);

        ASSERT_EQ(ga, 1);
        ASSERT_EQ(gb, 2.0f);
        ASSERT_EQ(gc, "3");

        file.close();
    }
}

TEST(SerializationTest, ClassFileSerializationTest)
{
    static std::string fileName = "TestGenerated/SerializationTest.ClassFileSerializationTest.bin";
    Common::PathUtils::MakeDirectoriesForFile(fileName);
    {
        Mirror::AutoCloseFileSerializeStream stream(fileName);

        TestStruct0 obj;
        obj.a = 1;
        obj.b = 2.0f;
        obj.c = "3";

        Mirror::Any ref = std::ref(obj);

        const auto& clazz = Mirror::Class::Get("TestStruct0");
        clazz.Serialize(stream, &ref);
    }

    {
        Mirror::AutoCloseFileDeserializeStream stream(fileName);

        const auto& clazz = Mirror::Class::Get("TestStruct0");
        Mirror::Any obj = clazz.GetConstructor(Mirror::NamePresets::defaultConstructor).ConstructOnStack();
        clazz.Deserailize(stream, &obj);

        const auto& ref = obj.CastTo<const TestStruct0&>();
        ASSERT_EQ(ref.a, 1);
        ASSERT_EQ(ref.b, 2.0f);
        ASSERT_EQ(ref.c, "3");
    }
}
