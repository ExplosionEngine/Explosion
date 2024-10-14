//
// Created by johnk on 2023/4/23.
//

#include <filesystem>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <Test/Test.h>
#include <Mirror/Mirror.h>
#include <SerializationTest.h>

int ga = 1;
float gb = 2.0f;
std::string gc = "3";

int gf(int a, int b)
{
    return a + b;
}

int SerializationTestStruct3::ga = 1;

int SerializationTestStruct3::gf(int a, int b)
{
    return a + b;
}

int SerializationTestStruct3::f() const // NOLINT
{
    return 1;
}

template <typename T>
void PerformSerializationTest(const std::filesystem::path& fileName, const T& object)
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

template <typename T>
void PerformJsonSerializationTest(const T& inValue, const std::string& inExceptJson)
{
    std::string json;
    {
        rapidjson::Document document;

        rapidjson::Value jsonValue;
        Common::JsonSerialize<T>(jsonValue, document.GetAllocator(), inValue);
        document.CopyFrom(jsonValue, document.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer writer(buffer);
        document.Accept(writer);

        json = std::string(buffer.GetString(), buffer.GetSize());
        if (!inExceptJson.empty()) {
            ASSERT_EQ(json, inExceptJson);
        }
    }

    {
        rapidjson::Document document;
        document.Parse(json.c_str());

        rapidjson::Value jsonValue;
        jsonValue.CopyFrom(document, document.GetAllocator());

        T value;
        Common::JsonDeserialize<T>(jsonValue, value);
        ASSERT_EQ(inValue, value);
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
    PerformSerializationTest(
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

    PerformSerializationTest(
        "../Test/Generated/Mirror/SerializationTest.ContainerFileSerializationTest.bin",
        obj);
}

TEST(SerializationTest, MetaObjectWithBaseClassTest)
{
    PerformSerializationTest(
        "../Test/Generated/Mirror/SerializationTest.MetaObjectWithBaseClassTest.bin",
        SerializationTestStruct2 { { 1, 2, "3.0" }, 4.0 });
}

TEST(SerializationTest, EnumSerializationTest)
{
    PerformSerializationTest<SerializationTestEnum>(
        "../Test/Generated/Mirror/SerializationTest.EnumSerializationTest.bin",
        SerializationTestEnum::b);
}

TEST(SerializationTest, ReflNodeSerializationTest)
{
    static std::filesystem::path fileName = "../Test/Generated/Mirror/SerializationTest.MetaTypeSerializationTest.bin";

    const auto& globalScope = GlobalScope::Get();
    PerformSerializationTest<const Variable*>(fileName, nullptr);
    PerformSerializationTest<const Variable*>(fileName, &globalScope.GetVariable("ga"));
    PerformSerializationTest<const Function*>(fileName, nullptr);
    PerformSerializationTest<const Function*>(fileName, &globalScope.GetFunction("gf"));

    const auto* clazz = &Class::Get<SerializationTestStruct3>();
    PerformSerializationTest<const Class*>(fileName, nullptr);
    PerformSerializationTest<const Class*>(fileName, clazz);
    PerformSerializationTest<const Constructor*>(fileName, nullptr);
    PerformSerializationTest<const Constructor*>(fileName, &clazz->GetDefaultConstructor());
    PerformSerializationTest<const Destructor*>(fileName, nullptr);
    PerformSerializationTest<const Destructor*>(fileName, &clazz->GetDestructor());
    PerformSerializationTest<const Variable*>(fileName, nullptr);
    PerformSerializationTest<const Variable*>(fileName, &clazz->GetStaticVariable("ga"));
    PerformSerializationTest<const Function*>(fileName, nullptr);
    PerformSerializationTest<const Function*>(fileName, &clazz->GetStaticFunction("gf"));
    PerformSerializationTest<const MemberVariable*>(fileName, nullptr);
    PerformSerializationTest<const MemberVariable*>(fileName, &clazz->GetMemberVariable("a"));
    PerformSerializationTest<const MemberFunction*>(fileName, nullptr);
    PerformSerializationTest<const MemberFunction*>(fileName, &clazz->GetMemberFunction("f"));

    const auto* enun = &Enum::Get<SerializationTestEnum>();
    PerformSerializationTest<const Enum*>(fileName, nullptr);
    PerformSerializationTest<const Enum*>(fileName, enun);
    PerformSerializationTest<const EnumValue*>(fileName, nullptr);
    PerformSerializationTest<const EnumValue*>(fileName, &enun->GetValue("a"));
}

TEST(SerializationTest, MetaObjectSerializationTest)
{
    PerformJsonSerializationTest<SerializationTestStruct2>(
        SerializationTestStruct2 { 1, 2.0f, "3", 4.0 },
        "");
}

TEST(SerializationTest, EnumJsonSerializationTest)
{
    PerformJsonSerializationTest<SerializationTestEnum>(
        SerializationTestEnum::b,
        R"(["SerializationTestEnum","b"])");
}

TEST(SerializationTest, MetaTypeJsonSerializationTest)
{
    const auto& globalScope = GlobalScope::Get();
    PerformJsonSerializationTest<const Variable*>(nullptr, R"(["",""])");
    PerformJsonSerializationTest<const Variable*>(&globalScope.GetVariable("ga"), R"(["","ga"])");
    PerformJsonSerializationTest<const Function*>(nullptr, R"(["",""])");
    PerformJsonSerializationTest<const Function*>(&globalScope.GetFunction("gf"), R"(["","gf"])");

    const auto* clazz = &Class::Get<SerializationTestStruct3>();
    PerformJsonSerializationTest<const Class*>(nullptr, R"("")");
    PerformJsonSerializationTest<const Class*>(clazz, R"("SerializationTestStruct3")");
    PerformJsonSerializationTest<const Constructor*>(nullptr, R"(["",""])");
    PerformJsonSerializationTest<const Constructor*>(&clazz->GetDefaultConstructor(), R"(["SerializationTestStruct3","_defaultCtor"])");
    PerformJsonSerializationTest<const Destructor*>(nullptr, R"("")");
    PerformJsonSerializationTest<const Destructor*>(&clazz->GetDestructor(), R"("SerializationTestStruct3")");
    PerformJsonSerializationTest<const Variable*>(nullptr, R"(["",""])");
    PerformJsonSerializationTest<const Variable*>(&clazz->GetStaticVariable("ga"), R"(["SerializationTestStruct3","ga"])");
    PerformJsonSerializationTest<const Function*>(nullptr, R"(["",""])");
    PerformJsonSerializationTest<const Function*>(&clazz->GetStaticFunction("gf"), R"(["SerializationTestStruct3","gf"])");
    PerformJsonSerializationTest<const MemberVariable*>(nullptr, R"(["",""])");
    PerformJsonSerializationTest<const MemberVariable*>(&clazz->GetMemberVariable("a"), R"(["SerializationTestStruct3","a"])");
    PerformJsonSerializationTest<const MemberFunction*>(nullptr, R"(["",""])");
    PerformJsonSerializationTest<const MemberFunction*>(&clazz->GetMemberFunction("f"), R"(["SerializationTestStruct3","f"])");

    const auto* enun = &Enum::Get<SerializationTestEnum>();
    PerformJsonSerializationTest<const Enum*>(nullptr, R"("")");
    PerformJsonSerializationTest<const Enum*>(enun, R"("SerializationTestEnum")");
    PerformJsonSerializationTest<const EnumValue*>(nullptr, R"(["",""])");
    PerformJsonSerializationTest<const EnumValue*>(&enun->GetValue("a"), R"(["SerializationTestEnum","a"])");
}
