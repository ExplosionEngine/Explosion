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

#if PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <Common/String.h>
#include <Common/Debug.h>

namespace Common {
    class NamedThread {
    public:
        NamedThread() = default;

        template <typename F, typename... Args>
        explicit NamedThread(const std::string& name, F&& task, Args&&... args)
        {
            thread = std::thread([this, task = std::forward<F>(task), name](Args&&... args) -> void {
                SetThreadName(name);
                task(args...);
            }, std::forward<Args>(args)...);
        }

        NamedThread(NamedThread&& other) noexcept : thread(std::move(other.thread)) {}

        ~NamedThread() = default;

        NamedThread& operator=(NamedThread&& other) noexcept
        {
            thread = std::move(other.thread);
            return *this;
        }

        void Join()
        {
            thread.join();
        }

    private:
        void SetThreadName(const std::string& name)
        {
#if PLATFORM_WINDOWS
            SetThreadDescription(thread.native_handle(), Common::StringUtils::ToWideString(name).c_str());
#elif PLATFORM_MACOS
            pthread_setname_np(name.c_str());
#else
            pthread_setname_np(thread.native_handle(), name.c_str());
#endif
        }

        std::thread thread;
    };

    class ThreadPool {
    public:
        ThreadPool(const std::string& name, uint8_t threadNum) : stop(false)
        {
            threads.reserve(threadNum);
            for (auto i = 0; i < threadNum; i++) {
                std::string fullName = name + "-" + std::to_string(i);
                threads.emplace_back(NamedThread(fullName, [this]() -> void {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(mutex);
                            condition.wait(lock, [this]() -> bool { return stop || !tasks.empty(); });
                            if (stop && tasks.empty()) {
                                return;
                            }
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                }));
            }
        }

        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                stop = true;
            }
            condition.notify_all();
            for (auto& thread : threads) {
                thread.Join();
            }
        }

        template <typename F, typename... Args>
        auto EmplaceTask(F&& task, Args&&... args)
        {
            using RetType = std::invoke_result_t<F, Args...>;
            auto packagedTask = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(task), std::forward<Args>(args)...));
            auto result = packagedTask->get_future();
            {
                std::unique_lock<std::mutex> lock(mutex);
                Assert(!stop);
                tasks.emplace([packagedTask]() -> void { (*packagedTask)(); });
            }
            condition.notify_one();
            return result;
        }

    private:
        bool stop;
        std::mutex mutex;
        std::condition_variable condition;
        std::vector<NamedThread> threads;
        std::queue<std::function<void()>> tasks;
    };
}
