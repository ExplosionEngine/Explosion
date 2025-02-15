//
// Created by johnk on 2022/7/20.
//

#pragma once

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <type_traits>

#include <Common/Debug.h>
#include <Common/Memory.h>
#include <Common/Utility.h>

namespace Common {
    class NamedThread {
    public:
        DefaultMovable(NamedThread)

        NamedThread();

        template <typename F> explicit NamedThread(const std::string& name, F&& task);

        void Join();

    private:
        void SetThreadName(const std::string& name);

        std::thread thread;
    };

    class ThreadPool {
    public:
        ThreadPool(const std::string& name, uint8_t threadNum);
        ~ThreadPool();

        template <typename F> auto EmplaceTask(F&& task);
        template <typename F> void ExecuteTasks(size_t taskNum, F&& task);

    private:
        template <typename Ret> auto EmplaceTaskInternal(Common::SharedPtr<std::packaged_task<Ret()>> packedTask);

        bool stop;
        std::mutex mutex;
        std::condition_variable condition;
        std::vector<NamedThread> threads;
        std::queue<std::function<void()>> tasks;
    };

    class WorkerThread {
    public:
        explicit WorkerThread(const std::string& name);
        ~WorkerThread();

        void Flush();

        template <typename F> auto EmplaceTask(F&& task);

    private:
        bool stop;
        bool flush;
        std::mutex mutex;
        std::condition_variable taskCondition;
        std::condition_variable flushCondition;
        NamedThread thread;
        std::queue<std::function<void()>> tasks;
    };
}

namespace Common {
    template <typename F>
    NamedThread::NamedThread(const std::string& name, F&& task)
    {
        thread = std::thread([this, task, name]() -> void {
            SetThreadName(name);
            task();
        });
    }

    template <typename F>
    auto ThreadPool::EmplaceTask(F&& task)
    {
        using RetType = std::invoke_result_t<F>;
        return EmplaceTaskInternal<RetType>(Common::MakeShared<std::packaged_task<RetType()>>(task));
    }

    template <typename F>
    void ThreadPool::ExecuteTasks(size_t taskNum, F&& task)
    {
        using RetType = std::invoke_result_t<F, size_t>;

        std::vector<std::future<RetType>> futures;
        futures.reserve(taskNum);
        for (size_t i = 0; i < taskNum; i++) {
            futures.emplace_back(EmplaceTaskInternal<RetType>(Common::MakeShared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(task), i))));
        }

        for (const auto& future : futures) {
            future.wait();
        }
    }

    template <typename Ret>
    auto ThreadPool::EmplaceTaskInternal(Common::SharedPtr<std::packaged_task<Ret()>> packedTask)
    {
        auto result = packedTask->get_future();
        {
            std::unique_lock lock(mutex);
            Assert(!stop);
            tasks.emplace([packedTask]() -> void { (*packedTask)(); });
        }
        condition.notify_one();
        return result;
    }

    template <typename F>
    auto WorkerThread::EmplaceTask(F&& task)
    {
        using RetType = std::invoke_result_t<F>;
        auto packagedTask = Common::MakeShared<std::packaged_task<RetType()>>(task);
        auto result = packagedTask->get_future();
        {
            std::unique_lock lock(mutex);
            Assert(!stop);
            tasks.emplace([packagedTask]() -> void { (*packagedTask)(); });
        }
        taskCondition.notify_one();
        return result;
    }
}
