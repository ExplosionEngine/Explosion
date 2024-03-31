//
// Created by johnk on 2023/4/17.
//

#include <RHI/Surface.h>

namespace RHI {
    SurfaceCreateInfo::SurfaceCreateInfo()
        : window(nullptr)
    {
    }

    SurfaceCreateInfo& SurfaceCreateInfo::Window(void* inWindow)
    {
        window = inWindow;
        return *this;
    }

    Surface::Surface(const SurfaceCreateInfo& createInfo) {}

    Surface::~Surface() = default;
}
