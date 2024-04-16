//
// Created by johnk on 2023/4/17.
//

#pragma once

#include <Common/Utility.h>

namespace RHI {
    struct SurfaceCreateInfo {
        void* window;

        SurfaceCreateInfo(void* inWindow = nullptr);
        SurfaceCreateInfo& SetWindow(void* inWindow);
    };

    class Surface {
    public:
        NonCopyable(Surface)
        virtual ~Surface();

    protected:
        explicit Surface(const SurfaceCreateInfo& createInfo);
    };
}
