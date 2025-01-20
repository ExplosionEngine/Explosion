//
// Created by johnk on 2025/1/17.
//

#include <Core/Thread.h>

namespace Core {
    static thread_local auto currentTag = ThreadTag::unknown;

    void ThreadContext::SetTag(ThreadTag inTag)
    {
        currentTag = inTag;
    }

    ThreadTag ThreadContext::GetTag()
    {
        return currentTag;
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
        tagToRestore = ThreadContext::GetTag();
        ThreadContext::SetTag(inTag);
    }

    ScopedThreadTag::~ScopedThreadTag()
    {
        ThreadContext::SetTag(tagToRestore);
    }
}
