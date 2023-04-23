//
// Created by johnk on 2023/4/17.
//

#include <RHI/Dummy/Surface.h>

namespace RHI::Dummy {
    DummySurface::DummySurface(const RHI::SurfaceCreateInfo& createInfo)
        : Surface(createInfo)
    {
    }

    DummySurface::~DummySurface() = default;

    void DummySurface::Destroy()
    {
        delete this;
    }
}
