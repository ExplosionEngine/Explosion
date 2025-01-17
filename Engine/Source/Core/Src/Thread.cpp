//
// Created by johnk on 2025/1/17.
//

#include <Core/Thread.h>

namespace Core {
    void ThreadContext::SetTag(ThreadTag inTag)
    {
        tag = inTag;
    }

    ThreadTag ThreadContext::GetTag()
    {
        return tag;
    }

    bool ThreadContext::IsUnknownThread()
    {
        return tag == ThreadTag::unknown;
    }

    bool ThreadContext::IsGameThread()
    {
        return tag == ThreadTag::game;
    }

    bool ThreadContext::IsRenderThread()
    {
        return tag == ThreadTag::render;
    }

    bool ThreadContext::IsGameWorkerThread()
    {
        return tag == ThreadTag::gameWorker;
    }

    bool ThreadContext::IsRenderWorkerThread()
    {
        return tag == ThreadTag::renderWorker;
    }

    thread_local ThreadTag ThreadContext::tag = ThreadTag::unknown;

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
