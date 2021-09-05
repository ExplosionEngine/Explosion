//
// Created by LiZhen on 2021/8/23.
//

#include <gtest/gtest.h>
#include <thread>
#include <Common/Templates/FixedSizeAllocator.h>

using namespace Explosion;

TEST(FixedSizeAllocatorTest, FixedSizeAllocatorTest1)
{
    std::atomic_int32_t t(0);

    std::thread t1([&t]() {
        for (int i = 0; i < 100; ++i) ++t;
    });

    std::thread t2([&t]() {
        for (int i = 0; i < 100; ++i) ++t;
    });

    std::thread t3([&t]() {
        for (int i = 0; i < 100; ++i) ++t;
    });

    t1.join();
    t2.join();
    t3.join();
    ASSERT_EQ(t, 300);
}

