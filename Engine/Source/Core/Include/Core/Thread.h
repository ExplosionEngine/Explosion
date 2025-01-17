//
// Created by johnk on 2025/1/17.
//

#pragma once

#include <cstdint>

namespace Core {
    enum class ThreadTag : uint8_t {
        unknown,
        game,
        render,
        gameWorker,
        renderWorker,
        max
    };

    class ThreadContext {
    public:
        static void SetTag(ThreadTag inTag);
        static ThreadTag GetTag();
        static bool IsUnknownThread();
        static bool IsGameThread();
        static bool IsRenderThread();
        static bool IsGameWorkerThread();
        static bool IsRenderWorkerThread();

    private:
        static thread_local ThreadTag tag;
    };

    class ScopedThreadTag {
    public:
        explicit ScopedThreadTag(ThreadTag inTag);
        ~ScopedThreadTag();

    private:
        ThreadTag tagToRestore;
    };
}
