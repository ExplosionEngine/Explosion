//
// Created by Kindem on 2025/3/1.
//

#pragma once

#include <functional>
#include <queue>
#include <future>

#include <Common/Debug.h>
#include <Common/Concurrent.h>
#include <Core/Thread.h>

namespace Runtime {
    class GameThread {
    public:
        static GameThread& Get();

        ~GameThread();

        template <typename F> auto EmplaceTask(F&& inTask);

    private:
        friend class Engine;

        GameThread();

        void Flush();

        std::mutex mutex;
        std::queue<std::function<void()>> tasks;
    };

    class GameWorkerThreads {
    public:
        static GameWorkerThreads& Get();

        ~GameWorkerThreads();

        void Start();
        void Stop();
        template <typename F> auto EmplaceTask(F&& inTask);
        template <typename F> void ExecuteTasks(size_t inTaskNum, F&& inTask);

    private:
        GameWorkerThreads();

        Common::UniquePtr<Common::ThreadPool> threads;
    };
}

namespace Runtime {
    template <typename F>
    auto GameThread::EmplaceTask(F&& inTask)
    {
        using RetType = std::invoke_result_t<F>;
        auto packagedTask = Common::MakeShared<std::packaged_task<RetType()>>(inTask);
        auto result = packagedTask->get_future();
        {
            std::unique_lock lock(mutex);
            tasks.emplace([packagedTask]() -> void { (*packagedTask)(); });
        }
        return result;
    }

    template <typename F>
    auto GameWorkerThreads::EmplaceTask(F&& inTask)
    {
        using RetType = std::invoke_result_t<F>;

        Assert(threads != nullptr);
        return threads->EmplaceTask([inTask]() -> RetType {
            Core::ScopedThreadTag tag(Core::ThreadTag::gameWorker);
            return inTask();
        });
    }

    template <typename F>
    void GameWorkerThreads::ExecuteTasks(size_t inTaskNum, F&& inTask)
    {
        Assert(threads != nullptr);
        auto reboundTask = std::bind(std::forward<F>(inTask), std::placeholders::_1);
        threads->ExecuteTasks(inTaskNum, [reboundTask](size_t inIndex) -> void {
            Core::ScopedThreadTag tag(Core::ThreadTag::gameWorker);
            reboundTask(inIndex);
        });
    }
} // namespace Runtime
