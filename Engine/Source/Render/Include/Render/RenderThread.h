//
// Created by johnk on 2022/8/7.
//

#pragma once

#include <Common/Debug.h>
#include <Common/Concurrent.h>
#include <Common/Memory.h>

namespace Render {
    class RenderThread {
    public:
        static RenderThread& Get()
        {
            static RenderThread instance;
            return instance;
        }

        RenderThread() = default;

        ~RenderThread() = default;

        void Start()
        {
            worker = Common::MakeUnique<Common::WorkerThread>("RenderThread");
        }

        void Flush()
        {
            Assert(worker != nullptr);
            worker->Flush();
        }

        template <typename F, typename... Args>
        auto EmplaceTask(F&& func, Args&&... args)
        {
            Assert(worker != nullptr);
            return worker->EmplaceTask(std::forward<F>(func), std::forward<Args>(args)...);
        }

    private:
        Common::UniqueRef<Common::WorkerThread> worker;
    };
}
