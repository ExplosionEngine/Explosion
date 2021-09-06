//
// Created by LiZhen on 2021/9/6.
//

#ifndef EXPLOSION_TASK_H
#define EXPLOSION_TASK_H

#include <cstdint>
#include <type_traits>

namespace Explosion {

    class Task {
    public:
        ~Task();

        using TaskFunc = void(*)(Task* task);

        template <typename Func>
        static constexpr Task* CreateTask(Func&& f)
        {
            Task* task = new Task();
            struct Stub {
                static constexpr void Call(Task* task) noexcept {
                    if constexpr (sizeof(Func) <= STORAGE_SIZE) {
                        Func& func = *reinterpret_cast<Func*>(&task->data.meta[0]);
                        func();
                    } else {
                        Func& func = *static_cast<Func*>(task->ptr);
                        func();
                    }
                }
            };
            task->func = &Stub::Call;

            if constexpr (sizeof(Func) <= STORAGE_SIZE) {
                new (&task->data.meta[0]) Func(std::move(f));
            } else {
                task->ptr = (void*)(new Func(std::move(f)));
            }
            return task;
        }

        void Execute()
        {
            if (func) func(this);
        }

    private:
        Task() = default;
        static constexpr uint32_t DEFAULT_SIZE = 64;
        static constexpr uint32_t STORAGE_SIZE = DEFAULT_SIZE - sizeof(TaskFunc);

        struct Storage {
            uint8_t meta[STORAGE_SIZE];
        };

        union {
            Storage data;
            void* ptr;
        };
        TaskFunc func;
    };

}

#endif//EXPLOSION_TASK_H
