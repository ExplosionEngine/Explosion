//
// Created by johnk on 2023/12/5.
//

#include <gtest/gtest.h>

#include <Common/Container.h>

TEST(ContainerTest, VectorSwapDeleteTest)
{
    std::vector<int> vec0 = { 1, 2, 3, 4, 5 };
    auto index = Common::VectorUtils::SwapWithLastAndDelete(vec0, 2);
    ASSERT_EQ(index, 2);
    ASSERT_EQ(vec0.size(), 4);
    ASSERT_EQ(vec0[0], 1);
    ASSERT_EQ(vec0[1], 2);
    ASSERT_EQ(vec0[2], 5);
    ASSERT_EQ(vec0[3], 4);

    std::vector<int> vec1 = { 1, 1, 2, 2, 3, 4, 4, 4 };
    for (auto i = 0; i < vec1.size();) {
        if (vec1[i] == 2) {
            i = Common::VectorUtils::SwapWithLastAndDelete(vec1, i);
        } else {
            i++;
        }
    }

    ASSERT_EQ(vec1.size(), 6);
    ASSERT_EQ(vec1[0], 1);
    ASSERT_EQ(vec1[1], 1);
    ASSERT_EQ(vec1[2], 4);
    ASSERT_EQ(vec1[3], 4);
    ASSERT_EQ(vec1[4], 3);
    ASSERT_EQ(vec1[5], 4);

    std::vector<int> vec2 = { 2, 2, 3, 3, 4, 4, 5 };
    auto iter = vec2.begin();
    while (iter != vec2.end()) {
        if (*iter == 3) {
            iter = Common::VectorUtils::SwapWithLastAndDelete(vec2, iter);
        } else {
            iter++;
        }
    }

    ASSERT_EQ(vec2.size(), 5);
    ASSERT_EQ(vec2[0], 2);
    ASSERT_EQ(vec2[1], 2);
    ASSERT_EQ(vec2[2], 5);
    ASSERT_EQ(vec2[3], 4);
    ASSERT_EQ(vec2[4], 4);
}

TEST(ContainerTest, VectorGetIntersection)
{
    std::vector<int> a = { 1, 2, 3, 4, 5 };
    std::vector<int> b = { 3, 4, 5, 6, 7 };
    std::vector<int> result = Common::VectorUtils::GetIntersection(a, b);

    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], 3);
    ASSERT_EQ(result[1], 4);
    ASSERT_EQ(result[2], 5);
}

TEST(ContainerTest, SetGetIntersection)
{
    std::unordered_set<int> a = { 1, 2, 3, 4, 5 };
    std::unordered_set<int> b = { 3, 4, 5, 6, 7 };
    std::unordered_set<int> result = Common::SetUtils::GetIntersection(a, b);

    ASSERT_EQ(result.size(), 3);
}
