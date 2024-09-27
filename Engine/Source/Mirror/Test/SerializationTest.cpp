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

template <typename T>
void PerformMetaObjectSerializationTest(const std::filesystem::path& fileName, const T& object)
{
    std::filesystem::create_directories(fileName.parent_path());
    {
        Common::BinaryFileSerializeStream stream(fileName.string());
        Serialize(stream, object);
    }

    {
        Common::BinaryFileDeserializeStream stream(fileName.string());

        T restored;
        Deserialize(stream, restored);
        ASSERT_EQ(restored, object);
    }
}

TEST(SerializationTest, VariableFileTest)
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

TEST(SerializationTest, ClassFileTest)
{
    PerformMetaObjectSerializationTest(
        "../Test/Generated/Mirror/SerializationTest.ClassFileSerializationTest.bin",
        SerializationTestStruct0 { 1, 2, "3.0" });
}

TEST(SerializationTest, ContainerFileTest)
{
    SerializationTestStruct1 obj;
    obj.a = { 1, 2 };
    obj.b = { "3", "4" };
    obj.c = { { 5, "6" }, { 7, "8" } };
    obj.d = { { false, true }, { true, false } };
    obj.e = { { 1, 2.0f, "3" } };

    PerformMetaObjectSerializationTest(
        "../Test/Generated/Mirror/SerializationTest.ContainerFileSerializationTest.bin",
        obj);
}

TEST(SerializationTest, MetaObjectWithBaseClassTest)
{
    PerformMetaObjectSerializationTest(
        "../Test/Generated/Mirror/SerializationTest.MetaObjectWithBaseClassTest.bin",
        SerializationTestStruct2 { { 1, 2, "3.0" }, 4.0 });
}

TEST(SerializationTest, EnumSerializationTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Mirror/SerializationTest.EnumSerializationTest.bin";
    std::filesystem::create_directories(fileName.parent_path());

    {
        Common::BinaryFileSerializeStream stream(fileName.string());
        Serialize(stream, SerializationTestEnum::b);
    }

    {
        Common::BinaryFileDeserializeStream stream(fileName.string());
        SerializationTestEnum metaEnum;
        Deserialize(stream, metaEnum);
        ASSERT_EQ(metaEnum, SerializationTestEnum::b);
    }
}

TEST(SerializationTest, MetaTypeSerializationTest)
{
    // TODO
}

TEST(SerializationTest, MetaObjectJsonSerializationTest)
{
    // TODO
}

TEST(SerializationTest, EnumJsonSerializationTest)
{
    // TODO
}

TEST(SerializationTest, MetaTypeJsonSerializationTest)
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
