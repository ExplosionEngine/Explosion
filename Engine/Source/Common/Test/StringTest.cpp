//
// Created by johnk on 2022/6/22.
//

#include <gtest/gtest.h>

#include <Common/String.h>

TEST(StringUtilsTest, ToUpperCaseTest)
{
    ASSERT_EQ(Common::StringUtils::ToUpperCase("abc"), "ABC");
    ASSERT_EQ(Common::StringUtils::ToUpperCase("temp"), "TEMP");
    ASSERT_EQ(Common::StringUtils::ToUpperCase("a,b,c"), "A,B,C");
}

TEST(StringUtilsTest, ToLowerCaseTest)
{
    ASSERT_EQ(Common::StringUtils::ToLowerCase("ABC"), "abc");
    ASSERT_EQ(Common::StringUtils::ToLowerCase("TEMP"), "temp");
    ASSERT_EQ(Common::StringUtils::ToLowerCase("A,B,C"), "a,b,c");
}

TEST(StringUtilsTest, SplitTest)
{
    auto result = Common::StringUtils::Split("hello world", " ");
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0], "hello");
    ASSERT_EQ(result[1], "world");
    
    result = Common::StringUtils::Split("a|b|c", "|");
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], "a");
    ASSERT_EQ(result[1], "b");
    ASSERT_EQ(result[2], "c");
    
    result = Common::StringUtils::Split("abacad", "a");
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0], "");
    ASSERT_EQ(result[1], "b");
    ASSERT_EQ(result[2], "c");
    ASSERT_EQ(result[3], "d");

    result = Common::StringUtils::Split("hello", " ");
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0], "hello");

    result = Common::StringUtils::Split("Test1, Test2, Test3(), Test4(A=2)", ", ");
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0], "Test1");
    ASSERT_EQ(result[1], "Test2");
    ASSERT_EQ(result[2], "Test3()");
    ASSERT_EQ(result[3], "Test4(A=2)");
}

TEST(StringUtilsTest, ReplaceTest)
{
    ASSERT_EQ(Common::StringUtils::Replace("hello", "el", "a"), "halo");
    ASSERT_EQ(Common::StringUtils::Replace("a|b|c|d|e", "|", ""), "abcde");
}

TEST(StringUtilsTest, RegexMatchTest)
{
    ASSERT_TRUE(Common::StringUtils::RegexMatch("Test(a)", ".+\\(.+\\)"));
    ASSERT_TRUE(Common::StringUtils::RegexMatch("abcd", "a.+d"));
    ASSERT_TRUE(Common::StringUtils::RegexMatch("zzzz", "z+"));
}

TEST(StringUtilsTest, RegexSearchFirstTest)
{
    ASSERT_EQ(Common::StringUtils::RegexSearchFirst("Test(a)", "\\(.+\\)"), "(a)");
    ASSERT_EQ(Common::StringUtils::RegexSearchFirst("2022, 2023, 2024", "\\d{4}"), "2022");
}

TEST(StringUtilsTest, RegexSearchTest)
{
    auto result = Common::StringUtils::RegexSearch("2022, 2023, 2024", "\\d{4}");
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], "2022");
    ASSERT_EQ(result[1], "2023");
    ASSERT_EQ(result[2], "2024");
}
