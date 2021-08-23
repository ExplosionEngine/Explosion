//
// Created by LiZhen on 2021/5/6.
//

#include <thread>
#include <mutex>

#ifndef EXPLOSION_NAMED_THREAD_H
#define EXPLOSION_NAMED_THREAD_H

namespace Explosion {

    class NamedThread {
    public:
        NamedThread(const std::string& name);
        ~NamedThread();

        template <typename F, typename ...Args>
        void StartThread(F&& func, Args&&... args)
        {
            if (thd) {
                return;
            }
            thd = std::make_unique<std::thread>(
                &RunableThread::Wrap<F, Args...>,
                this,
                std::forward<F>(func),
                std::forward<Args>(args)...);
        }

    private:
        template <typename F, typename ...Args>
        void Wrap(F&& f, Args&&... args)
        {
            do {
                std::invoke(f, args...);
                std::unique_lock<Mutex> lock(mutex);
                if (!exist.load()) {
                    cv.wait(lock);
                }
            } while (!exist.load());
        }

        std::unique_ptr<std::thread> thd;
        std::atomic_bool exist = false;
        mutable std::mutex mtx;
        mutable std::condition_variable cv;
    };

}

#endif