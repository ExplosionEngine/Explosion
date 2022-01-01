//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_SURFACE_H
#define EXPLOSION_RHI_SURFACE_H

#include <Common/Utility.h>

namespace RHI {
    struct WindowsSurfaceCreateInfo {
        void* hWnd;
        void* hInstance;
    };

    struct SurfaceCreateInfo {
        WindowsSurfaceCreateInfo* windows;
    };

    class Surface {
    public:
        NON_COPYABLE(Surface)
        virtual ~Surface();

    protected:
        explicit Surface(const SurfaceCreateInfo& createInfo);
    };
}

#endif //EXPLOSION_RHI_SURFACE_H
