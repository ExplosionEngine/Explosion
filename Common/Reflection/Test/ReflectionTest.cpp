//
// Created by Zach Lee on 2021/7/21.
//
#include <gtest/gtest.h>
#include <Reflection/Reflection.h>
#include <Reflection/JsonSerialization.h>

using namespace Explosion;
using namespace entt::literals;

struct Test1 {
    uint32_t a;
    uint64_t b;
};

struct Test2 {
    float a;
    Test1 b;
};

struct TestStruct {
    float a;
    double b;
    int c;
    char d;
    Test2 e;
};

class ReflectionTest : public testing::Test {
  protected:
    static void SetUpTestSuite()
    {
        Reflection::Factory<Test1>()
            .type("Test1")
            .ctor<>()
            .ctor<uint32_t, uint64_t>()
            .data<&Test1::a>("a")
            .data<&Test1::b>("b");

        Reflection::Factory<Test2>()
            .type("Test2")
            .ctor<>()
            .ctor<float, Test1>()
            .data<&Test2::a>("a")
            .data<&Test2::b>("b");

        Reflection::Factory<TestStruct>()
            .type("TestStruct")
            .ctor<>()
            .ctor<float, double, int, char, Test2>()
            .data<&TestStruct::a>("a")
            .data<&TestStruct::b>("b")
            .data<&TestStruct::c>("c")
            .data<&TestStruct::d>("d")
            .data<&TestStruct::e>("e");
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ReflectionTest, SimpleReflectionTest)
{
    Test2 tmp = {1.f, {2, 3}};

    auto type = Reflection::Resolve("TestStruct");
    auto any = type.construct(1.f, 0.0, 1, 'a', tmp);
    TestStruct* data = any.try_cast<TestStruct>();
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(data->a, 1.f);
    ASSERT_EQ(data->b, 0.0);
    ASSERT_EQ(data->c, 1);
    ASSERT_EQ(data->d, 'a');
    ASSERT_EQ(data->e.a, 1.f);
    ASSERT_EQ(data->e.b.a, 2);
    ASSERT_EQ(data->e.b.b, 3);
}

TEST_F(ReflectionTest, JsonReflectionTest)
{
    auto test1Type = Reflection::Resolve("Test1");
    Test1 t = test1Type.construct().cast<Test1>();
    t.a = 2;
    t.b = 3;
    Test2 tmp = {1.f, t};
    auto type = Reflection::Resolve("TestStruct");
    auto any = type.construct(1.f, 0.0, 1, 'a', tmp);

    JsonSerialization::ToJson("TestStruct.json", any);

    auto rst = JsonSerialization::FromJson("TestStruct.json").cast<TestStruct>();
    ASSERT_EQ(rst.a, 1.f);
    ASSERT_EQ(rst.b, 0.0);
    ASSERT_EQ(rst.c, 1);
    ASSERT_EQ(rst.d, 'a');
    ASSERT_EQ(rst.e.a, 1.f);
    ASSERT_EQ(rst.e.b.a, 2);
    ASSERT_EQ(rst.e.b.b, 3);
}