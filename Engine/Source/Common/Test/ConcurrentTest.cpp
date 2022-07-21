//
// Created by johnk on 2022/7/20.
//

#include <gtest/gtest.h>

#include <Common/Concurrent.h>

TEST(ConcurrentTest, NamedThreadTest)
{
    std::atomic<uint32_t> value = 0;
    Common::NamedThread thread("TestThread", [&value]() -> void { value++; });
    value++;
    thread.Join();
    ASSERT_EQ(value, 2);
}

TEST(ConcurrentTest, ThreadPoolTest0)
{
    Common::ThreadPool threadPool("TestThreadPool", 4);
    std::vector<std::future<bool>> futures(20);
    for (auto i = 0; i < 20; i++) {
        futures[i] = threadPool.EmplaceTask([]() -> bool { return true; });
    }
    for (auto& future : futures) {
        future.wait();
    }
}

TEST(ConcurrentTest, ThreadPoolTest1)
{
    Common::ThreadPool threadPool("TestThreadPool", 4);
    std::atomic<uint32_t> count = 0;
    std::vector<std::future<bool>> futures(20);
    for (auto i = 0; i < 20; i++) {
        futures[i] = threadPool.EmplaceTask([&count]() -> bool { count++; return true; });
    }
    for (auto& future : futures) {
        future.wait();
    }
    ASSERT_EQ(count, 20);
}

TEST(ConcurrentTest, ThreadPoolTest2)
{
    std::atomic<uint32_t> count = 0;
    {
        Common::ThreadPool threadPool("TestThreadPool", 4);
        for (auto i = 0; i < 20; i++) {
            threadPool.EmplaceTask([&count]() -> bool { count++; return true; });
        }
    }
    ASSERT_EQ(count, 20);
}

TEST(ConcurrentTest, ThreadPoolTest3)
{
    std::atomic<uint32_t> count = 0;
    {
        Common::ThreadPool threadPool("TestThreadPool", 1);
        for (auto i = 0; i < 200; i++) {
            threadPool.EmplaceTask([&count]() -> bool { count++; return true; });
        }
    }
    ASSERT_EQ(count, 200);
}