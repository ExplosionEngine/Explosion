//
// Created by johnk on 2023/12/5.
//

#include <Test/Test.h>
#include <Common/Container.h>
using namespace Test;

TEST(ContainerTest, VectorSwapDeleteTest)
{
    std::vector vec0 = { 1, 2, 3, 4, 5 };
    const auto index = Common::VectorUtils::SwapWithLastAndDelete(vec0, 2);
    ASSERT_EQ(index, 2);
    AssertVecEq(vec0, std::vector { 1, 2, 5, 4 });

    std::vector vec1 = { 1, 1, 2, 2, 3, 4, 4, 4 };
    for (size_t i = 0; i < vec1.size();) {
        if (vec1[i] == 2) {
            i = Common::VectorUtils::SwapWithLastAndDelete(vec1, i);
        } else {
            i++;
        }
    }
    AssertVecEq(vec1, std::vector { 1, 1, 4, 4, 3, 4 });

    std::vector vec2 = { 2, 2, 3, 3, 4, 4, 5 };
    auto iter = vec2.begin();
    while (iter != vec2.end()) {
        if (*iter == 3) {
            iter = Common::VectorUtils::SwapWithLastAndDelete(vec2, iter);
        } else {
            ++iter;
        }
    }
    AssertVecEq(vec2, std::vector { 2, 2, 5, 4, 4 });
}

TEST(ContainerTest, VectorGetIntersection)
{
    const std::vector a = { 1, 2, 3, 4, 5 };
    const std::vector b = { 3, 4, 5, 6, 7 };
    const auto result = Common::VectorUtils::GetIntersection(a, b);

    AssertVecEq(result, std::vector { 3, 4, 5 });
}

TEST(ContainerTest, SetGetIntersection)
{
    const std::unordered_set a = { 1, 2, 3, 4, 5 };
    const std::unordered_set b = { 3, 4, 5, 6, 7 };
    const auto result = Common::SetUtils::GetIntersection(a, b);

    ASSERT_EQ(result.size(), 3);
}

TEST(ContainerTest, TrunkBasic)
{
    // TODO
}
