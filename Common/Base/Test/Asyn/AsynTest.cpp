//
// Created by LiZhen on 2021/9/6.
//

#include <gtest/gtest.h>
#include <thread>
#include <Common/Asyn/NamedThread.h>
#include <Common/Asyn/Task.h>

using namespace Explosion;

void func() {}

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

    struct Test {
        uint8_t data[64];
        float v = 2.0f;
    };

    int v1 = 1;
    Test v2;
    auto task1 = Task::CreateTask([v1]() {
        std::cout << v1 << std::endl;
    });

    auto task2 = Task::CreateTask([v2]() {
      std::cout << v2.v << std::endl;
    });
    task1->Execute();
    task2->Execute();
}