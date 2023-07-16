//
// Created by johnk on 2022/7/3.
//

#include <string_view>

#include <gtest/gtest.h>

#include <Common/Hash.h>

TEST(HashTest, CityHashTest)
{
    std::string_view testString = "Hello, World";
    ASSERT_EQ(4508847345234768385, Common::HashUtils::CityHash(testString.data(), testString.size()));
}

TEST(HashTest, StrCrc32Test)
{
    ASSERT_EQ(Common::HashUtils::StrCrc32("hello"), 0x3610a686);
    ASSERT_EQ(Common::HashUtils::StrCrc32("explosion game engine"), 0xdb39167f);
}
