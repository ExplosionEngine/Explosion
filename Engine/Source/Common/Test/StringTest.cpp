//
// Created by johnk on 2022/6/22.
//

#include <Test/Test.h>

#include <Common/String.h>
using namespace Common;

TEST(StringUtilsTest, ToUpperCaseTest)
{
    ASSERT_EQ(StringUtils::ToUpperCase("abc"), "ABC");
    ASSERT_EQ(StringUtils::ToUpperCase("temp"), "TEMP");
    ASSERT_EQ(StringUtils::ToUpperCase("a,b,c"), "A,B,C");
}

TEST(StringUtilsTest, ToLowerCaseTest)
{
    ASSERT_EQ(StringUtils::ToLowerCase("ABC"), "abc");
    ASSERT_EQ(StringUtils::ToLowerCase("TEMP"), "temp");
    ASSERT_EQ(StringUtils::ToLowerCase("A,B,C"), "a,b,c");
}

TEST(StringUtilsTest, SplitTest)
{
    auto result = StringUtils::Split("hello world", " ");
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0], "hello");
    ASSERT_EQ(result[1], "world");
    
    result = StringUtils::Split("a|b|c", "|");
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], "a");
    ASSERT_EQ(result[1], "b");
    ASSERT_EQ(result[2], "c");
    
    result = StringUtils::Split("abacad", "a");
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0], "");
    ASSERT_EQ(result[1], "b");
    ASSERT_EQ(result[2], "c");
    ASSERT_EQ(result[3], "d");

    result = StringUtils::Split("hello", " ");
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0], "hello");

    result = StringUtils::Split("Test1, Test2, Test3(), Test4(A=2)", ", ");
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0], "Test1");
    ASSERT_EQ(result[1], "Test2");
    ASSERT_EQ(result[2], "Test3()");
    ASSERT_EQ(result[3], "Test4(A=2)");
}

TEST(StringUtilsTest, ReplaceTest)
{
    ASSERT_EQ(StringUtils::Replace("hello", "el", "a"), "halo");
    ASSERT_EQ(StringUtils::Replace("a|b|c|d|e", "|", ""), "abcde");
}

TEST(StringUtilsTest, RegexMatchTest)
{
    ASSERT_TRUE(StringUtils::RegexMatch("Test(a)", ".+\\(.+\\)"));
    ASSERT_TRUE(StringUtils::RegexMatch("abcd", "a.+d"));
    ASSERT_TRUE(StringUtils::RegexMatch("zzzz", "z+"));
}

TEST(StringUtilsTest, RegexSearchFirstTest)
{
    ASSERT_EQ(StringUtils::RegexSearchFirst("Test(a)", "\\(.+\\)"), "(a)");
    ASSERT_EQ(StringUtils::RegexSearchFirst("2022, 2023, 2024", "\\d{4}"), "2022");
}

TEST(StringUtilsTest, RegexSearchTest)
{
    const auto result = StringUtils::RegexSearch("2022, 2023, 2024", "\\d{4}");
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], "2022");
    ASSERT_EQ(result[1], "2023");
    ASSERT_EQ(result[2], "2024");
}

TEST(StringUtilsTest, AfterFirstTest)
{
    ASSERT_EQ(StringUtils::AfterFirst("Hello, World", ", "), "World");
    ASSERT_EQ(StringUtils::AfterFirst("12, 34, 56", ", "), "34, 56");
}

TEST(StringUtilsTest, BeforeFirstTest)
{
    ASSERT_EQ(StringUtils::BeforeFirst("Hello, World", ", "), "Hello");
    ASSERT_EQ(StringUtils::BeforeFirst("12, 34, 56", ", "), "12");
}

TEST(StringUtilsTest, AfterLastTest)
{
    ASSERT_EQ(StringUtils::AfterLast("Hello, World", ", "), "World");
    ASSERT_EQ(StringUtils::AfterLast("12, 34, 56", ", "), "56");
}

TEST(StringUtilsTest, BeforeLastTest)
{
    ASSERT_EQ(StringUtils::BeforeLast("Hello, World", ", "), "Hello");
    ASSERT_EQ(StringUtils::BeforeLast("12, 34, 56", ", "), "12, 34");
}

TEST(StringUtilsTest, ToStringTest)
{
    ASSERT_EQ(Common::ToString(true), "true");
    ASSERT_EQ(Common::ToString(false), "false");
    ASSERT_EQ(Common::ToString(1), "1");

    const std::string v0 = "1";
    ASSERT_EQ(Common::ToString(v0), "1");

    std::optional<int> v1 {};
    ASSERT_EQ(Common::ToString(v1), "nullopt");
    v1 = 1;
    ASSERT_EQ(Common::ToString(v1), "1");

    const std::pair<int, bool> v2 = { 1, false }; // NOLINT
    ASSERT_EQ(Common::ToString(v2), "1: false");

    const std::vector<int> v3 = { 1, 2, 3 };
    ASSERT_EQ(Common::ToString(v3), "(1, 2, 3)");
}
