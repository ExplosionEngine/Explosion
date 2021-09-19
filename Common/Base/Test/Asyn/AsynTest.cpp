//
// Created by Zach Lee on 2021/9/6.
//

#include <gtest/gtest.h>
#include <future>
#include <Common/Asyn/NamedThread.h>
#include <Common/Asyn/Task.h>
#include <Common/Asyn/TaskQueue.h>

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
}

class TestThread {
public:
    explicit TestThread(std::string_view name)
        : thread(name)
    {

    }
    ~TestThread() = default;

    void Start()
    {
        thread.StartThread(&TestThread::Main, this);
    }

    void Stop()
    {
        thread.ExitThread();
    }

    void Main()
    {
        taskQueue.Execute();
    }

    template <typename Func>
    void Enqueue(Func&& f)
    {
        taskQueue.template Emplace(std::forward<Func>(f));
    }

    void Wait()
    {
        if (future.valid()) {
            future.wait();
        }
    }

    void Flush()
    {
        struct Functor {
            std::promise<void> promise;
            void operator() () {
                promise.set_value();
            }
        };
        Functor f;
        future = f.promise.get_future();
        taskQueue.Emplace(std::move(f));
        thread.Notify();
    }

private:
    NamedThread thread;
    TaskQueue taskQueue;
    std::future<void> future;
};

TEST(NamedThreadTest, NamedThreadTest2)
{
    TestThread testThread("test");

    testThread.Start();

    testThread.Wait();
    int a = 0;
    int b = 0;

    testThread.Enqueue([&a]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        a = 1;
    });

    testThread.Enqueue([&b]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        b = 2;
    });

    ASSERT_EQ(a, 0);
    ASSERT_EQ(b, 0);
    testThread.Flush();
    testThread.Wait();
    ASSERT_EQ(a, 1);
    ASSERT_EQ(b, 2);

    testThread.Enqueue([&a]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        a = 3;
    });

    testThread.Enqueue([&b]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        b = 4;
    });
    testThread.Flush();
    testThread.Wait();
    ASSERT_EQ(a, 3);
    ASSERT_EQ(b, 4);

    testThread.Stop();
}

TEST(TaskQueueTest, TaskQueueTest1)
{
    TaskQueue taskQueue;
    int a = 0;
    int b = 0;

    taskQueue.Emplace([&a]() {
        a = 1;
    });

    taskQueue.Emplace([&b]() {
        b = 2;
    });

    ASSERT_EQ(a, 0);
    ASSERT_EQ(b, 0);
    taskQueue.Execute();
    ASSERT_EQ(a, 1);
    ASSERT_EQ(b, 2);
}