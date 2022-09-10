//
// Created by johnk on 2022/9/5.
//

#include <gtest/gtest.h>

#include <Mirror/Any.h>

struct TestStruct0 {
    int intValue;
    float floatValue;
};

struct TestStruct1 {
    explicit TestStruct1(std::vector<int>&& inValues) : values(std::move(inValues)) {}
    TestStruct1(const TestStruct1& inOther) = default;
    TestStruct1(TestStruct1&& inOther) noexcept : values(std::move(inOther.values)) {}

    std::vector<int> values;
};

TEST(AnyTest, ValueAssignTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(a0.CastTo<int>(), 1);

    const int v1 = 2;
    Mirror::Any a1 = v1;
    ASSERT_EQ(a1.CastTo<const int&>(), 2);

    TestStruct0 v2 = { 1, 2.0f };
    Mirror::Any a2 = v2;
    auto& r2 = a2.CastTo<TestStruct0&>();
    ASSERT_EQ(r2.intValue, 1);
    ASSERT_EQ(r2.floatValue, 2.0f);

    r2.intValue = 3;
    ASSERT_EQ(v2.intValue, 1);

    TestStruct1 v3({ 1, 2 });
    Mirror::Any a3 = std::move(v3);
    const auto& r3 = a3.CastTo<const TestStruct1&>();
    ASSERT_EQ(r3.values[0], 1);
    ASSERT_EQ(r3.values[1], 2);
}

TEST(AnyTest, ValueConstructTest)
{
    Mirror::Any a0(1);
    ASSERT_EQ(a0.CastTo<int>(), 1);

    const int v1 = 2;
    Mirror::Any a1(v1);
    ASSERT_EQ(a1.CastTo<int>(), 2);
}

TEST(AnyTest, ValueReAssignTest)
{
    Mirror::Any a0 = 1;
    a0 = 2.0f;
    ASSERT_EQ(a0.CastTo<float>(), 2.0f);
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

TEST(AnyTest, RefConstructTest)
{
    int v0 = 1;
    Mirror::Any a0(std::ref(v0));
    ASSERT_EQ(a0.CastTo<const int&>(), 1);

    const int v1 = 2;
    Mirror::Any a1(std::ref(v1));
    ASSERT_EQ(a1.CastTo<const int&>(), 2);
}

TEST(AnyTest, RefReAssignTest)
{
    int v0 = 1;
    Mirror::Any a0 = std::ref(v0);
    float v1 = 2.0f;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.CastTo<const float&>(), 2.0f);
}

TEST(AnyTest, BlendReAssignTest)
{
    Mirror::Any a0 = 1;
    float v1 = 2.0f;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.CastTo<const float&>(), 2.0f);
}

TEST(AnyTest, SizeTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(a0.Size(), sizeof(int));

    float v1 = 2.0f;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.Size(), sizeof(std::reference_wrapper<float>));
}

TEST(AnyTest, DataTest)
{
    TestStruct0 v0 = { 1, 2.0f };
    Mirror::Any a0 = v0;
    ASSERT_NE(a0.Data(), &v0);
}

TEST(AnyTest, IsReferenceTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(a0.IsReference(), false);

    float v2 = 2.0f;
    a0 = std::ref(v2);
    ASSERT_EQ(a0.IsReference(), true);
}

TEST(AnyTest, CastableTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(a0.CanCastTo<int>(), true);
    ASSERT_EQ(a0.CanCastTo<const int>(), true);
    ASSERT_EQ(a0.CanCastTo<int&>(), true);
    ASSERT_EQ(a0.CanCastTo<const int&>(), true);

    int v0 = 2;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.CanCastTo<int>(), true);
    ASSERT_EQ(a0.CanCastTo<const int>(), true);
    ASSERT_EQ(a0.CanCastTo<int&>(), true);
    ASSERT_EQ(a0.CanCastTo<const int&>(), true);

    const int v1 = 3;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.CanCastTo<int>(), false);
    ASSERT_EQ(a0.CanCastTo<const int>(), true);
    ASSERT_EQ(a0.CanCastTo<int&>(), false);
    ASSERT_EQ(a0.CanCastTo<const int&>(), true);
}

TEST(AnyTest, CastToTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(a0.CastTo<int>(), 1);
    ASSERT_EQ(a0.CastTo<const int>(), 1);
    ASSERT_EQ(a0.CastTo<int&>(), 1);
    ASSERT_EQ(a0.CastTo<const int&>(), 1);

    int v0 = 2;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.CastTo<int>(), 2);
    ASSERT_EQ(a0.CastTo<const int>(), 2);
    ASSERT_EQ(a0.CastTo<int&>(), 2);
    ASSERT_EQ(a0.CastTo<const int&>(), 2);

    const int v1 = 3;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.CastTo<const int>(), 3);
    ASSERT_EQ(a0.CastTo<const int&>(), 3);
}

TEST(AnyTest, TryCastTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(*a0.TryCastTo<int>(), 1);
    ASSERT_EQ(*a0.TryCastTo<const int>(), 1);
    ASSERT_EQ(a0.TryCastTo<float>(), nullptr);
}
