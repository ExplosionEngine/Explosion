//
// Created by johnk on 2025/1/17.
//

#include <Core/Thread.h>

namespace Core {
    static thread_local auto currentTag = ThreadTag::unknown;
    static thread_local uint64_t frameNumber = 0;

    void ThreadContext::SetTag(ThreadTag inTag)
    {
        currentTag = inTag;
    }

    void ThreadContext::IncFrameNumber()
    {
        frameNumber++;
    }

    ThreadTag ThreadContext::Tag()
    {
        return currentTag;
    }

    uint64_t ThreadContext::FrameNumber()
    {
        return frameNumber;
    }

    bool ThreadContext::IsUnknownThread()
    {
        return currentTag == ThreadTag::unknown;
    }

    bool ThreadContext::IsGameThread()
    {
        return currentTag == ThreadTag::game;
    }

    bool ThreadContext::IsRenderThread()
    {
        return currentTag == ThreadTag::render;
    }

    bool ThreadContext::IsGameWorkerThread()
    {
        return currentTag == ThreadTag::gameWorker;
    }

    bool ThreadContext::IsRenderWorkerThread()
    {
        return currentTag == ThreadTag::renderWorker;
    }

    bool ThreadContext::IsGameOrWorkerThread()
    {
        return currentTag == ThreadTag::game || currentTag == ThreadTag::gameWorker;
    }

    bool ThreadContext::IsRenderOrWorkerThread()
    {
        return currentTag == ThreadTag::render || currentTag == ThreadTag::renderWorker;
    }

    ScopedThreadTag::ScopedThreadTag(ThreadTag inTag)
    {
        tagToRestore = ThreadContext::Tag();
        ThreadContext::SetTag(inTag);
    }

    ScopedThreadTag::~ScopedThreadTag()
    {
        ThreadContext::SetTag(tagToRestore);
    }
}
