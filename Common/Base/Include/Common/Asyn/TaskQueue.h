//
// Created by Zach Lee on 2021/9/7.
//

#ifndef EXPLOSION_TASKQUEUE_H
#define EXPLOSION_TASKQUEUE_H

#include <Common/Templates/Allocators.h>
#include <Common/Asyn/Task.h>

namespace Explosion {

    class TaskQueue {
    public:
        TaskQueue() = default;
        ~TaskQueue() = default;

        template <typename Func>
        void Emplace(Func&& f)
        {
            Task* task = new (allocator.Allocate()) Task();
            struct Stub {
                static constexpr void Call(Task* task) noexcept {
                    if constexpr (sizeof(Func) <= Task::STORAGE_SIZE) {
                        Func* func = reinterpret_cast<Func*>(&task->data.meta[0]);
                        (*func)();
                        func->~Func();
                    } else {
                        Func* func = static_cast<Func*>(task->ptr);
                        (*func)();
                        delete func;
                        task->ptr = nullptr;
                    }
                }
            };
            task->func = &Stub::Call;

            if constexpr (sizeof(Func) <= Task::STORAGE_SIZE) {
                new (&task->data.meta[0]) Func(std::move(f));
            } else {
                task->ptr = (void*)(new Func(std::move(f)));
            }
            tasks.emplace_back(task);
        }

        void Execute()
        {
            for (auto& task : tasks) {
                task->Execute();
                task->~Task();
                allocator.Free(static_cast<void*>(task));
            }
            tasks.clear();
        }

    private:
        using Allocator = FreeListFixedSizeAllocator<sizeof(Task), 1024>;
        std::vector<Task*> tasks;
        Allocator allocator;
    };

}

#endif // EXPLOSION_TASKQUEUE_H
