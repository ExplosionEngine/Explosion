//
// Created by johnk on 2023/4/17.
//

#pragma once

#include <Common/Utility.h>

namespace RHI {
    struct SurfaceCreateInfo {
        void* window;
    };

    class Surface {
    public:
        NonCopyable(Surface)
        virtual ~Surface();

        virtual void Destroy() = 0;

    protected:
        explicit Surface(const SurfaceCreateInfo& createInfo);
    };
}
