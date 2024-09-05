//
// Created by johnk on 2023/4/23.
//

#include <filesystem>

#include <Test/Test.h>

#include <Mirror/Mirror.h>
#include <SerializationTest.h>

int ga = 0;
float gb = 0.0f;
std::string gc;

TEST(SerializationTest, VariableFileSerializationTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Mirror/SerializationTest.VariableFileSerializationTest.bin";
    std::filesystem::create_directories(fileName.parent_path());
    {
        Common::BinaryFileSerializeStream stream(fileName.string());

        ga = 1;
        gb = 2.0f;
        gc = "3";

        const auto& globalScope = Mirror::GlobalScope::Get();
        globalScope.GetVariable("ga").GetDyn().Serialize(stream);
        globalScope.GetVariable("gb").GetDyn().Serialize(stream);
        globalScope.GetVariable("gc").GetDyn().Serialize(stream);
    }

    {
        ga = 4;
        gb = 5.0f;
        gc = "6";

        Common::BinaryFileDeserializeStream stream(fileName.string());

        const auto& globalScope = Mirror::GlobalScope::Get();
        globalScope.GetVariable("ga").GetDyn().Deserialize(stream);
        globalScope.GetVariable("gb").GetDyn().Deserialize(stream);
        globalScope.GetVariable("gc").GetDyn().Deserialize(stream);

        ASSERT_EQ(ga, 1);
        ASSERT_EQ(gb, 2.0f);
        ASSERT_EQ(gc, "3");
    }
}

TEST(SerializationTest, ClassFileSerializationTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Mirror/SerializationTest.ClassFileSerializationTest.bin";
    std::filesystem::create_directories(fileName.parent_path());
    {
        Common::BinaryFileSerializeStream stream(fileName.string());

        SerializationTestStruct0 obj;
        obj.a = 1;
        obj.b = 2.0f;
        obj.c = "3";
        Serialize(stream, obj);
    }

    {
        Common::BinaryFileDeserializeStream stream(fileName.string());

        SerializationTestStruct0 obj;
        Deserialize(stream, obj);

        const auto& [a, b, c] = obj;
        ASSERT_EQ(a, 1);
        ASSERT_EQ(b, 2.0f);
        ASSERT_EQ(c, "3");
    }
}

TEST(SerializationTest, ContainerFileSerializationTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Mirror/SerializationTest.ContainerFileSerializationTest.bin";
    std::filesystem::create_directories(fileName.parent_path());
    const auto& clazz = Mirror::Class::Get("SerializationTestStruct1");
    {
        Common::BinaryFileSerializeStream stream(fileName.string());

        SerializationTestStruct1 obj;
        obj.a = { 1, 2 };
        obj.b = { "3", "4" };
        obj.c = { { 5, "6" }, { 7, "8" } };
        obj.d = { { false, true }, { true, false } };

        Mirror::Any(std::ref(obj))
            .Serialize(stream);
    }

    {
        Common::BinaryFileDeserializeStream stream(fileName.string());

        Mirror::Any ref = clazz.GetDefaultConstructor().Construct();
        ref.Deserialize(stream);

        const auto& [a, b, c, d] = ref.As<const SerializationTestStruct1&>();
        ASSERT_EQ(a.size(), 2);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(b.size(), 2);
        ASSERT_EQ(b.contains("3"), true);
        ASSERT_EQ(b.contains("4"), true);
        ASSERT_EQ(c.size(), 2);
        ASSERT_EQ(c.at(5), "6");
        ASSERT_EQ(c.at(7), "8");
        ASSERT_EQ(d.size(), 2);
        ASSERT_EQ(d[0].size(), 2);
        ASSERT_EQ(d[0][0], false);
        ASSERT_EQ(d[0][1], true);
        ASSERT_EQ(d[1].size(), 2);
        ASSERT_EQ(d[1][0], true);
        ASSERT_EQ(d[1][1], false);
    }
}

TEST(SerializationTest, MetaObjectTypeSerializationTest)
{
    // TODO
}

TEST(SerializationTest, EnumSerializationTest)
{
    // TODO
}

TEST(SerializationTest, TransientTest)
{
    // TODO
}

TEST(SerializationTest, CallbackTest)
{
    // TODO
}
