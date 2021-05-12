//
// Created by LiZhen on 2021/5/12.
//

#include <gtest/gtest.h>
#include <Explosion/Render/FrameGraph/FgResources.h>

using namespace Explosion;

TEST(FrameGraphTest, FrameGraphResourceHandleTest1)
{
    FgResourceHandle res0;
    ASSERT_EQ(!!res0, false);

    FgResourceHandle res1(0);
    ASSERT_EQ(!!res1, true);
    ASSERT_EQ(res1.Index(), 0);
    ASSERT_EQ(res1.Version(), 0);

    ++res1;
    ASSERT_EQ(!!res1, true);
    ASSERT_EQ(res1.Index(), 0);
    ASSERT_EQ(res1.Version(), 1);

    res1.Reset();
    ASSERT_EQ(!!res1, false);
}

TEST(FrameGraphTest, FrameGraphResourceHandleTest2)
{
    FgResourceHandle<uint64_t> res0;
    ASSERT_EQ(!!res0, false);

    FgResourceHandle res1(0);
    ASSERT_EQ(!!res1, true);
    ASSERT_EQ(res1.Index(), 0);
    ASSERT_EQ(res1.Version(), 0);

    ++res1;
    ASSERT_EQ(!!res1, true);
    ASSERT_EQ(res1.Index(), 0);
    ASSERT_EQ(res1.Version(), 1);

    res1.Reset();
    ASSERT_EQ(!!res1, false);
}