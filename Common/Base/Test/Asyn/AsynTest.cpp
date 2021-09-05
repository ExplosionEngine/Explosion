//
// Created by LiZhen on 2021/9/6.
//

#include <gtest/gtest.h>
#include <thread>
#include <Common/Asyn/NamedThread.h>

using namespace Explosion;

TEST(NamedThreadTest, NamedThreadTest1)
{
    int val = 0;
    NamedThread thd("test thread");
    auto id = std::this_thread::get_id();
    thd.StartThread([&val, &id]() {
        auto tid = std::this_thread::get_id();
        ASSERT_NE(id, tid);
        for (int i = 0; i < 10; ++i) {
            val++;
        }
    });

    thd.ExitThread();
    ASSERT_EQ(val, 10);
}