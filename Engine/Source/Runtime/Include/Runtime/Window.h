#pragma once

#include <Common/Utility.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API Window {
    public:
        virtual ~Window();

        NonCopyable(Window)
        NonMovable(Window)

        virtual bool ShouldClose() const = 0;
        virtual void RequestClose() const = 0;
        virtual void Tick(float inDeltaSeconds) = 0;
        virtual void Render(float inDeltaSeconds) = 0;
        virtual void WaitIdle() const = 0;

    protected:
        Window();
    };
}
