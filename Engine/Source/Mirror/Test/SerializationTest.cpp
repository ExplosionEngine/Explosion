//
// Created by johnk on 2023/4/23.
//

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>

#include <Test/Test.h>

#include <Mirror/Registry.h>
#include <Mirror/Mirror.h>

int ga;
float gb;
std::string gc;

struct SerializationTestStruct0 {
    int a;
    float b;
    std::string c;
};

struct SerializationTestStruct1 {
    std::vector<int> a;
    std::unordered_set<std::string> b;
    std::unordered_map<int, std::string> c;
    std::vector<std::vector<bool>> d;
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
            .Class<SerializationTestStruct0>("SerializationTestStruct0")
                .MemberVariable<&SerializationTestStruct0::a>("a")
                .MemberVariable<&SerializationTestStruct0::b>("b")
                .MemberVariable<&SerializationTestStruct0::c>("c");

        Mirror::Registry::Get()
            .Class<SerializationTestStruct1>("SerializationTestStruct1")
                .MemberVariable<&SerializationTestStruct1::a>("a")
                .MemberVariable<&SerializationTestStruct1::b>("b")
                .MemberVariable<&SerializationTestStruct1::c>("c")
                .MemberVariable<&SerializationTestStruct1::d>("d");
    }
};
static MirrorInfoRegistry registry;

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
        globalScope.GetVariable("ga").SerializeDyn(stream);
        globalScope.GetVariable("gb").SerializeDyn(stream);
        globalScope.GetVariable("gc").SerializeDyn(stream);
    }

    {
        ga = 4;
        gb = 5.0f;
        gc = "6";

        Common::BinaryFileDeserializeStream stream(fileName.string());

        const auto& globalScope = Mirror::GlobalScope::Get();
        globalScope.GetVariable("ga").DeserializeDyn(stream);
        globalScope.GetVariable("gb").DeserializeDyn(stream);
        globalScope.GetVariable("gc").DeserializeDyn(stream);

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

        const auto& clazz = Mirror::Class::Get("SerializationTestStruct0");
        clazz.SerializeDyn(stream, Mirror::Any(std::ref(obj)));
    }

    {
        Common::BinaryFileDeserializeStream stream(fileName.string());

        const auto& clazz = Mirror::Class::Get("SerializationTestStruct0");
        Mirror::Any obj = clazz.GetDefaultConstructor().Construct();
        clazz.DeserailizeDyn(stream, obj);

        const auto& [a, b, c] = obj.As<const SerializationTestStruct0&>();
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

        clazz.SerializeDyn(stream, Mirror::Any(std::ref(obj)));
    }

    {
        Common::BinaryFileDeserializeStream stream(fileName.string());

        Mirror::Any ref = clazz.GetDefaultConstructor().Construct();
        clazz.DeserailizeDyn(stream, ref);

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
