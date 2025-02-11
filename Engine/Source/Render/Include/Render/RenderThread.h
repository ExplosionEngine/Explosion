//
// Created by johnk on 2025/2/11.
//

#pragma once

#include <Common/Memory.h>
#include <Common/Concurrent.h>
#include <Core/Thread.h>

namespace Render {
    class RenderThread {
    public:
        static RenderThread& Get();

        ~RenderThread();

        void Start();
        void Stop();
        void Flush() const;
        template <typename F, typename... Args> auto EmplaceTask(F&& inTask, Args&&... inArgs);

    private:
        RenderThread();

        Common::UniqueRef<Common::WorkerThread> thread;
    };

    class RenderWorkerThreads {
    public:
        static RenderWorkerThreads& Get();

        ~RenderWorkerThreads();

        void Start();
        void Stop();
        template <typename F, typename... Args> auto EmplaceTask(F&& inTask, Args&&... inArgs);
        template <typename F, typename... Args> void ExecuteTasks(size_t inTaskNum, F&& inTask, Args&&... inArgs);

    private:
        RenderWorkerThreads();

        Common::UniqueRef<Common::ThreadPool> threads;
    };
}

namespace Render {
    template <typename F, typename ... Args>
    auto RenderThread::EmplaceTask(F&& inTask, Args&&... inArgs)
    {
        Assert(thread != nullptr);
        return thread->EmplaceTask(std::forward<F>(inTask), std::forward<Args>(inArgs)...);
    }

    template <typename F, typename ... Args>
    auto RenderWorkerThreads::EmplaceTask(F&& inTask, Args&&... inArgs)
    {
        using RetType = std::invoke_result_t<F, Args...>;

        Assert(threads != nullptr);
        auto packedTask = std::bind(std::forward<F>(inTask), std::forward<Args>(inArgs)...);
        return threads->EmplaceTask([packedTask]() -> RetType {
            Core::ThreadContext::SetTag(Core::ThreadTag::renderWorker);
            return packedTask();
        });
    }

    template <typename F, typename ... Args>
    void RenderWorkerThreads::ExecuteTasks(size_t inTaskNum, F&& inTask, Args&&... inArgs)
    {
        Assert(threads != nullptr);
        auto packedTask = std::bind(std::forward<F>(inTask), std::placeholders::_1, std::forward<Args>(inArgs)...);
        threads->ExecuteTasks(inTaskNum, [packedTask](size_t inIndex) -> void {
            Core::ThreadContext::SetTag(Core::ThreadTag::renderWorker);
            packedTask(inIndex);
        });
    }
}
