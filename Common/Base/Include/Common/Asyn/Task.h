//
// Created by LiZhen on 2021/9/6.
//

#ifndef EXPLOSION_TASK_H
#define EXPLOSION_TASK_H

#include <cstdint>
#include <type_traits>
#include <Common/Templates/Allocators.h>

namespace Explosion {

    class Task {
    public:
        ~Task() = default;

        using TaskFunc = void(*)(Task* task);

        void Execute()
        {
            if (func) func(this);
        }

    private:
        Task() = default;
        friend class TaskQueue;

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
