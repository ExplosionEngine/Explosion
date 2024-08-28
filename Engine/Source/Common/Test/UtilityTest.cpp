//
// Created by johnk on 2024/5/4.
//

#include <Test/Test.h>

#include <Common/Utility.h>
using namespace Common;

TEST(UtilityTest, AlignUpTest)
{
    ASSERT_EQ(AlignUp<4>(3), 4);
    ASSERT_EQ(AlignUp<4>(7), 8);
    ASSERT_EQ(AlignUp<256>(258), 512);
}
