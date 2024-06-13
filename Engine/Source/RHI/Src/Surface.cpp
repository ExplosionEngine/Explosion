//
// Created by johnk on 2023/4/17.
//

#include <RHI/Surface.h>

namespace RHI {
    SurfaceCreateInfo::SurfaceCreateInfo(void* inWindow)
        : window(inWindow)
    {
    }

    SurfaceCreateInfo& SurfaceCreateInfo::SetWindow(void* inWindow)
    {
        window = inWindow;
        return *this;
    }

    Surface::Surface(const SurfaceCreateInfo&) {}

    Surface::~Surface() = default;
}
