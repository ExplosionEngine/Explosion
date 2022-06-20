//
// Created by johnk on 2022/6/12.
//

#include <meta/factory.hpp>
#include <meta/meta.hpp>
#include <gtest/gtest.h>

#include <Common/Meta.h>

int v0 = 1;
float v1 = 2.0f;

struct GlobalScope;

int registry = []() -> int {
    std::hash<std::string_view> hash {};

    meta::reflect<GlobalScope>(hash("Global"))
        .data<&v0>(hash("v0"))
        .data<&v1>(hash("v1"));

    return 0;
}();

TEST(MetaTest, GlobalVariableTest)
{
    std::hash<std::string_view> hash {};

    auto globalFactory = meta::resolve(hash("Global"));

    meta::any value = globalFactory.data(hash("v0")).get({});
    ASSERT_EQ(value.cast<int>(), 1);

    value = globalFactory.data(hash("v1")).get({});
    ASSERT_EQ(value.cast<float>(), 2.0f);
}
