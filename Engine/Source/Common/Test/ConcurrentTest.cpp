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

TEST(ConcurrentTest, ThreadPoolTest)
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
