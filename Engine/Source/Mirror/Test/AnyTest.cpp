//
// Created by johnk on 2022/9/5.
//

#include <gtest/gtest.h>

#include <Mirror/Any.h>

struct TestStruct {
    int intValue;
    float floatValue;
};

TEST(AnyTest, ValueAssignTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(a0.CastTo<int>(), 1);

    const int v1 = 2;
    Mirror::Any a1 = v1;
    ASSERT_EQ(a1.CastTo<const int&>(), 2);

    TestStruct v2 = { 1, 2.0f };
    Mirror::Any a2 = v2;
    auto& r2 = a2.CastTo<TestStruct&>();
    ASSERT_EQ(r2.intValue, 1);
    ASSERT_EQ(r2.floatValue, 2.0f);

    r2.intValue = 3;
    ASSERT_EQ(v2.intValue, 1);
}

TEST(AnyTest, RefAssignTest)
{
    int v0 = 1;
    Mirror::Any a0 = std::ref(v0);
    ASSERT_EQ(a0.CastTo<int&>(), 1);
    a0.CastTo<int&>() = 2;
    ASSERT_EQ(v0, 2);

    const int v1 = 2;
    Mirror::Any a1 = std::ref(v1);
    ASSERT_EQ(a1.CastTo<const int&>(), 2);
}
