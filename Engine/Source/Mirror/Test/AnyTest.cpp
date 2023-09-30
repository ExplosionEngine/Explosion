//
// Created by johnk on 2022/9/5.
//

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <gtest/gtest.h>

#include <Mirror/Any.h>

struct AnyTestStruct0 {
    int intValue;
    float floatValue;
};

struct AnyTestStruct1 {
    explicit AnyTestStruct1(std::vector<int>&& inValues) : values(std::move(inValues)) {}
    AnyTestStruct1(const AnyTestStruct1& inOther) = default;
    AnyTestStruct1(AnyTestStruct1&& inOther) noexcept : values(std::move(inOther.values)) {}

    std::vector<int> values;
};

TEST(AnyTest, ValueAssignTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(a0.As<int>(), 1);

    const int v1 = 2;
    Mirror::Any a1 = v1;
    ASSERT_EQ(a1.As<const int&>(), 2);

    AnyTestStruct0 v2 = { 1, 2.0f };
    Mirror::Any a2 = v2;
    auto& r2 = a2.As<AnyTestStruct0&>();
    ASSERT_EQ(r2.intValue, 1);
    ASSERT_EQ(r2.floatValue, 2.0f);

    r2.intValue = 3;
    ASSERT_EQ(v2.intValue, 1);

    AnyTestStruct1 v3({ 1, 2 });
    Mirror::Any a3 = std::move(v3);
    const auto& r3 = a3.As<const AnyTestStruct1&>();
    ASSERT_EQ(r3.values[0], 1);
    ASSERT_EQ(r3.values[1], 2);
}

TEST(AnyTest, ValueConstructTest)
{
    Mirror::Any a0(1);
    ASSERT_EQ(a0.As<int>(), 1);

    const int v1 = 2;
    Mirror::Any a1(v1);
    ASSERT_EQ(a1.As<const int>(), 2);
}

TEST(AnyTest, ValueReAssignTest)
{
    Mirror::Any a0 = 1;
    a0 = 2.0f;
    ASSERT_EQ(a0.As<float>(), 2.0f);
}

TEST(AnyTest, RefAssignTest)
{
    int v0 = 1;
    Mirror::Any a0 = std::ref(v0);
    ASSERT_EQ(a0.As<int&>(), 1);
    a0.As<int&>() = 2;
    ASSERT_EQ(v0, 2);

    const int v1 = 2;
    Mirror::Any a1 = std::ref(v1);
    ASSERT_EQ(a1.As<const int&>(), 2);
}

TEST(AnyTest, RefConstructTest)
{
    int v0 = 1;
    Mirror::Any a0(std::ref(v0));
    ASSERT_EQ(a0.As<const int&>(), 1);

    const int v1 = 2;
    Mirror::Any a1(std::ref(v1));
    ASSERT_EQ(a1.As<const int&>(), 2);
}

TEST(AnyTest, RefReAssignTest)
{
    int v0 = 1;
    Mirror::Any a0 = std::ref(v0);
    float v1 = 2.0f;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.As<const float&>(), 2.0f);
}

TEST(AnyTest, BlendReAssignTest)
{
    Mirror::Any a0 = 1;
    float v1 = 2.0f;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.As<const float&>(), 2.0f);
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
    AnyTestStruct0 v0 = { 1, 2.0f };
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
    ASSERT_EQ(a0.Castable<int>(), true);
    ASSERT_EQ(a0.Castable<const int>(), true);
    ASSERT_EQ(a0.Castable<int&>(), true);
    ASSERT_EQ(a0.Castable<const int&>(), true);

    const int v0 = 2;
    a0 = v0;
    ASSERT_EQ(a0.Castable<int>(), false);
    ASSERT_EQ(a0.Castable<const int>(), true);
    ASSERT_EQ(a0.Castable<int&>(), false);
    ASSERT_EQ(a0.Castable<const int&>(), true);

    int v1 = 3;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.Castable<int>(), true);
    ASSERT_EQ(a0.Castable<const int>(), true);
    ASSERT_EQ(a0.Castable<int&>(), true);
    ASSERT_EQ(a0.Castable<const int&>(), true);

    const int v2 = 4;
    a0 = std::ref(v2);
    ASSERT_EQ(a0.Castable<int>(), false);
    ASSERT_EQ(a0.Castable<const int>(), true);
    ASSERT_EQ(a0.Castable<int&>(), false);
    ASSERT_EQ(a0.Castable<const int&>(), true);
}

TEST(AnyTest, ConstCastable)
{
    const Mirror::Any a0 = 1;
    ASSERT_EQ(a0.Castable<int>(), false);
    ASSERT_EQ(a0.Castable<const int>(), true);
    ASSERT_EQ(a0.Castable<int&>(), false);
    ASSERT_EQ(a0.Castable<const int&>(), true);

    const int v0 = 2;
    const Mirror::Any a1 = v0;
    ASSERT_EQ(a1.Castable<int>(), false);
    ASSERT_EQ(a1.Castable<const int>(), true);
    ASSERT_EQ(a1.Castable<int&>(), false);
    ASSERT_EQ(a1.Castable<const int&>(), true);

    int v1 = 3;
    const Mirror::Any a2 = std::ref(v1);
    ASSERT_EQ(a2.Castable<int>(), true);
    ASSERT_EQ(a2.Castable<const int>(), true);
    ASSERT_EQ(a2.Castable<int&>(), true);
    ASSERT_EQ(a2.Castable<const int&>(), true);

    const int v2 = 4;
    const Mirror::Any a3 = std::ref(v2);
    ASSERT_EQ(a3.Castable<int>(), false);
    ASSERT_EQ(a3.Castable<const int>(), true);
    ASSERT_EQ(a3.Castable<int&>(), false);
    ASSERT_EQ(a3.Castable<const int&>(), true);
}

TEST(AnyTest, AsTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(a0.As<int>(), 1);
    ASSERT_EQ(a0.As<const int>(), 1);
    ASSERT_EQ(a0.As<int&>(), 1);
    ASSERT_EQ(a0.As<const int&>(), 1);

    const int v0 = 2;
    a0 = v0;
    ASSERT_EQ(a0.As<const int>(), 2);
    ASSERT_EQ(a0.As<const int&>(), 2);

    int v1 = 3;
    a0 = std::ref(v1);
    ASSERT_EQ(a0.As<int>(), 3);
    ASSERT_EQ(a0.As<const int>(), 3);
    ASSERT_EQ(a0.As<int&>(), 3);
    ASSERT_EQ(a0.As<const int&>(), 3);

    const int v2 = 4;
    a0 = std::ref(v2);
    ASSERT_EQ(a0.As<const int>(), 4);
    ASSERT_EQ(a0.As<const int&>(), 4);
}

TEST(AnyTest, ConstAsTest)
{
    const Mirror::Any a0 = 1;
    ASSERT_EQ(a0.As<const int>(), 1);
    ASSERT_EQ(a0.As<const int&>(), 1);

    const int v0 = 2;
    const Mirror::Any a1 = v0;
    ASSERT_EQ(a1.As<const int>(), 2);
    ASSERT_EQ(a1.As<const int&>(), 2);

    int v1 = 3;
    const Mirror::Any a2 = std::ref(v1);
    ASSERT_EQ(a2.As<int>(), 3);
    ASSERT_EQ(a2.As<const int>(), 3);
    ASSERT_EQ(a2.As<int&>(), 3);
    ASSERT_EQ(a2.As<const int&>(), 3);

    const int v2 = 4;
    const Mirror::Any a3 = std::ref(v2);
    ASSERT_EQ(a3.As<const int>(), 4);
    ASSERT_EQ(a3.As<const int&>(), 4);
}

TEST(AnyTest, TryAsTest)
{
    Mirror::Any a0 = 1;
    ASSERT_EQ(*a0.TryAs<int>(), 1);
    ASSERT_EQ(*a0.TryAs<const int>(), 1);
    ASSERT_EQ(a0.TryAs<float>(), nullptr);
}

TEST(AnyTest, ConstTryAsTest)
{
    const int v0 = 2;
    Mirror::Any a0 = v0;
    ASSERT_EQ(a0.TryAs<int>(), nullptr);
    ASSERT_EQ(*a0.TryAs<const int>(), 2);
    ASSERT_EQ(a0.TryAs<float>(), nullptr);
}

TEST(AnyTest, StringTypeTest)
{
    Mirror::Any a0(std::string("hello"));
    ASSERT_EQ(a0.Castable<std::string>(), true);
    ASSERT_EQ(a0.Castable<std::string&>(), true);
    ASSERT_EQ(a0.Castable<const std::string&>(), true);
    ASSERT_EQ(a0.As<const std::string&>(), "hello");

    Mirror::Any a1 = std::string("world");
    ASSERT_EQ(a1.As<const std::string&>(), "world");

    std::string s0 = "test";
    Mirror::Any a2 = std::ref(s0);
    ASSERT_EQ(a2.As<std::string>(), "test");
    a2.As<std::string&>() = "test2";
    ASSERT_EQ(a2.As<const std::string&>(), "test2");
    s0 = "test3";
    ASSERT_EQ(a2.As<const std::string&>(), "test3");
}

TEST(AnyTest, ContainerTest)
{
    std::vector<int> v0 = { 1, 2 };
    Mirror::Any a0 = v0;
    ASSERT_EQ(a0.As<const std::vector<int>&>().size(), 2);
    ASSERT_EQ(a0.As<const std::vector<int>&>()[0], 1);
    ASSERT_EQ(a0.As<const std::vector<int>&>()[1], 2);

    std::vector<float> v1 = { 3.0f, 4.0f };
    Mirror::Any a1 = std::ref(v1);
    ASSERT_EQ(a1.As<std::vector<float>&>().size(), 2);
    ASSERT_EQ(a1.As<std::vector<float>&>()[0], 3.0f);
    ASSERT_EQ(a1.As<std::vector<float>&>()[1], 4.0f);
    v1.emplace_back(5.0f);
    ASSERT_EQ(a1.As<std::vector<float>&>().size(), 3);
    ASSERT_EQ(a1.As<std::vector<float>&>()[2], 5.0f);

    std::unordered_set<std::string> v2 = { "7", "8" };
    Mirror::Any a2 = std::ref(v2);
    ASSERT_EQ(a2.As<const std::unordered_set<std::string>&>().size(), 2);
    ASSERT_EQ(a2.As<const std::unordered_set<std::string>&>().contains("7"), true);
    ASSERT_EQ(a2.As<const std::unordered_set<std::string>&>().contains("8"), true);

    std::unordered_map<int, std::string> v3 = { { 9, "10" }, { 11, "12" } };
    Mirror::Any a3 = v3;
    ASSERT_EQ((a3.As<const std::unordered_map<int, std::string>&>().size()), 2);
    ASSERT_EQ((a3.As<const std::unordered_map<int, std::string>&>().find(9)->second), "10");
    ASSERT_EQ((a3.As<const std::unordered_map<int, std::string>&>().find(11)->second), "12");
}
