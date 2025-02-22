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
        template <typename F> auto EmplaceTask(F&& inTask);

    private:
        RenderThread();

        Common::UniquePtr<Common::WorkerThread> thread;
    };

    class RenderWorkerThreads {
    public:
        static RenderWorkerThreads& Get();

        ~RenderWorkerThreads();

        void Start();
        void Stop();
        template <typename F> auto EmplaceTask(F&& inTask);
        template <typename F> void ExecuteTasks(size_t inTaskNum, F&& inTask);

    private:
        RenderWorkerThreads();

        Common::UniquePtr<Common::ThreadPool> threads;
    };
}

namespace Render {
    template <typename F>
    auto RenderThread::EmplaceTask(F&& inTask)
    {
        Assert(thread != nullptr);
        return thread->EmplaceTask(std::forward<F>(inTask));
    }

    template <typename F>
    auto RenderWorkerThreads::EmplaceTask(F&& inTask)
    {
        using RetType = std::invoke_result_t<F>;

        Assert(threads != nullptr);
        auto packedTask = std::bind(std::forward<F>(inTask));
        return threads->EmplaceTask([packedTask]() -> RetType {
            Core::ScopedThreadTag tag(Core::ThreadTag::renderWorker);
            return packedTask();
        });
    }

    template <typename F>
    void RenderWorkerThreads::ExecuteTasks(size_t inTaskNum, F&& inTask)
    {
        Assert(threads != nullptr);
        auto packedTask = std::bind(std::forward<F>(inTask), std::placeholders::_1);
        threads->ExecuteTasks(inTaskNum, [packedTask](size_t inIndex) -> void {
            Core::ScopedThreadTag tag(Core::ThreadTag::renderWorker);
            packedTask(inIndex);
        });
    }
}
