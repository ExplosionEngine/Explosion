//
// Created by johnk on 2022/7/20.
//

#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <future>
#include <memory>
#include <functional>
#include <type_traits>

#include <Common/String.h>
#include <Common/Debug.h>
#include <Common/Utility.h>

namespace Common {
    class NamedThread {
    public:
        DefaultMovable(NamedThread)

        NamedThread();

        template <typename F, typename... Args>
        explicit NamedThread(const std::string& name, F&& task, Args&&... args);

        void Join();

    private:
        void SetThreadName(const std::string& name);

        std::thread thread;
    };

    class ThreadPool {
    public:
        ThreadPool(const std::string& name, uint8_t threadNum);
        ~ThreadPool();

        template <typename F, typename... Args>
        auto EmplaceTask(F&& task, Args&&... args);

    private:
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

        template <typename F, typename... Args>
        auto EmplaceTask(F&& task, Args&&... args);

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
    template <typename F, typename... Args>
    NamedThread::NamedThread(const std::string& name, F&& task, Args&& ... args)
    {
        thread = std::thread([this, task = std::forward<F>(task), name](Args&&... args) -> void {
            SetThreadName(name);
            task(args...);
        }, std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    auto ThreadPool::EmplaceTask(F&& task, Args&& ... args)
    {
        using RetType = std::invoke_result_t<F, Args...>;
        auto packagedTask = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(task), std::forward<Args>(args)...));
        auto result = packagedTask->get_future();
        {
            std::unique_lock lock(mutex);
            Assert(!stop);
            tasks.emplace([packagedTask]() -> void { (*packagedTask)(); });
        }
        condition.notify_one();
        return result;
    }

    template <typename F, typename... Args>
    auto WorkerThread::EmplaceTask(F&& task, Args&& ... args)
    {
        using RetType = std::invoke_result_t<F, Args...>;
        auto packagedTask = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(task), std::forward<Args>(args)...));
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
