//
// Created by Kindem on 2025/3/1.
//

#include <Runtime/GameThread.h>

namespace Runtime {
    GameThread& GameThread::Get()
    {
        static GameThread instance;
        return instance;
    }

    GameThread::GameThread() = default;

    GameThread::~GameThread()
    {
        Flush();
    }

    void GameThread::Flush()
    {
        std::queue<std::function<void()>> tasksToExecute;
        {
            std::unique_lock lock(mutex);
            tasksToExecute.swap(tasks);
        }

        while (!tasksToExecute.empty()) {
            tasksToExecute.front()();
            tasksToExecute.pop();
        }
    }

    GameWorkerThreads& GameWorkerThreads::Get()
    {
        static GameWorkerThreads instance;
        return instance;
    }


    GameWorkerThreads::GameWorkerThreads() = default;

    GameWorkerThreads::~GameWorkerThreads() = default;

    void GameWorkerThreads::Start()
    {
        Assert(threads == nullptr);
        threads = Common::MakeUnique<Common::ThreadPool>("GameWorkers", 8);
    }

    void GameWorkerThreads::Stop()
    {
        Assert(threads != nullptr);
        threads = nullptr;
    }
} // namespace Runtime
