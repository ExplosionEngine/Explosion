//
// Created by johnk on 2025/1/17.
//

#pragma once

#include <cstdint>

#include <Core/Api.h>

namespace Core {
    enum class ThreadTag : uint8_t {
        unknown,
        game,
        render,
        gameWorker,
        renderWorker,
        max
    };

    class CORE_API ThreadContext {
    public:
        static void SetTag(ThreadTag inTag);
        static void IncFrameNumber();

        static ThreadTag Tag();
        static uint64_t FrameNumber();
        static bool IsUnknownThread();
        static bool IsGameThread();
        static bool IsRenderThread();
        static bool IsGameWorkerThread();
        static bool IsRenderWorkerThread();
        static bool IsGameOrWorkerThread();
        static bool IsRenderOrWorkerThread();
    };

    class CORE_API ScopedThreadTag {
    public:
        explicit ScopedThreadTag(ThreadTag inTag);
        ~ScopedThreadTag();

    private:
        ThreadTag tagToRestore;
    };
}
