//
// Created by johnk on 2025/2/11.
//

#include <Render/RenderThread.h>

namespace Render {
    RenderThread& RenderThread::Get()
    {
        static RenderThread instance;
        return instance;
    }

    RenderThread::RenderThread() = default;

    RenderThread::~RenderThread() = default;

    void RenderThread::Start()
    {
        Assert(thread == nullptr);
        thread = Common::MakeUnique<Common::WorkerThread>("RenderingThread");
        thread->EmplaceTask([]() -> void { Core::ThreadContext::SetTag(Core::ThreadTag::render); });
    }

    void RenderThread::Stop()
    {
        Assert(thread != nullptr);
        thread = nullptr;
    }

    void RenderThread::Flush() const
    {
        Assert(thread != nullptr);
        thread->Flush();
    }

    RenderWorkerThreads& RenderWorkerThreads::Get()
    {
        static RenderWorkerThreads instance;
        return instance;
    }

    RenderWorkerThreads::RenderWorkerThreads() = default;

    RenderWorkerThreads::~RenderWorkerThreads() = default;

    void RenderWorkerThreads::Start()
    {
        Assert(threads == nullptr);
        threads = Common::MakeUnique<Common::ThreadPool>("RenderWorkers", 8);
    }

    void RenderWorkerThreads::Stop()
    {
        Assert(threads != nullptr);
        threads = nullptr;
    }
}
