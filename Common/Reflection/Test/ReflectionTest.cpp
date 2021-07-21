//
// Created by LiZhen on 2021/7/21.
//
#include <gtest/gtest.h>
#include <Reflection/Reflection.h>
#include <Reflection/JsonSerialization.h>

using namespace Explosion;
using namespace entt::literals;

struct TestStruct {
    float a;
    double b;
    int c;
    char d;
};

class ReflectionTest : public testing::Test {
  protected:
    void SetUp() override
    {
        Reflection::Factory<TestStruct>()
            .type("Test"_hs)
            .ctor<float, double, int, char>()
            .data<&TestStruct::a>("a"_hs)
            .data<&TestStruct::b>("b"_hs)
            .data<&TestStruct::c>("c"_hs)
            .data<&TestStruct::d>("d"_hs);
    }

    void TearDown() override
    {
    }
};

TEST_F(ReflectionTest, SimpleReflectionTest)
{
    auto type = Reflection::Resolve("Test"_hs);
    auto any = type.construct(1.f, 0.0, 1, 'a');
    TestStruct* data = any.try_cast<TestStruct>();
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(data->a, 1.f);
    ASSERT_EQ(data->b, 0.0);
    ASSERT_EQ(data->c, 1);
    ASSERT_EQ(data->d, 'a');
}

TEST_F(ReflectionTest, JsonReflectionTest)
{
    auto type = Reflection::Resolve("Test"_hs);
    auto any = type.construct(1.f, 0.0, 1, 'a');

    JsonSerialization::ToJson("test.json", any);
}