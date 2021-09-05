//
// Created by LiZhen on 2021/5/6.
//

#ifndef EXPLOSION_NAMED_THREAD_H
#define EXPLOSION_NAMED_THREAD_H

#include <thread>
#include <mutex>
#include <string_view>
#include <Common/Templates/Allocators.h>

namespace Explosion {

    class NamedThread {
    public:
        NamedThread(const std::string_view& name);
        ~NamedThread();

        template <typename F, typename ...Args>
        void StartThread(F&& func, Args&&... args)
        {
            if (thd) {
                return;
            }
            thd = std::make_unique<std::thread>(
                &NamedThread::Wrap<F, Args...>,
                this,
                std::forward<F>(func),
                std::forward<Args>(args)...);
        }

        void ExitThread();

        void Notify();

    private:
        template <typename F, typename ...Args>
        void Wrap(F&& f, Args&&... args)
        {
            do {
                std::invoke(f, args...);
                std::unique_lock<std::mutex> lock(mtx);
                if (!exit.load()) {
                    cv.wait(lock);
                }
            } while (!exit.load());
        }

        std::string_view name;
        std::unique_ptr<std::thread> thd;
        std::atomic_bool exit = false;
        mutable std::mutex mtx;
        mutable std::condition_variable cv;
    };

}

#endif