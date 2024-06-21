//
// Created by johnk on 2023/4/17.
//

#include <RHI/Dummy/Surface.h>

namespace RHI::Dummy {
    DummySurface::DummySurface(const SurfaceCreateInfo& createInfo)
        : Surface(createInfo)
    {
    }

    DummySurface::~DummySurface() = default;
}
