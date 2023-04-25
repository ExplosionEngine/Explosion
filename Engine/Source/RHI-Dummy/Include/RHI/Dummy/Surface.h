//
// Created by johnk on 2023/4/17.
//

#pragma once

#include <RHI/Surface.h>

namespace RHI::Dummy {
    class DummySurface : public Surface {
    public:
        NON_COPYABLE(DummySurface)
        explicit DummySurface(const SurfaceCreateInfo& createInfo);
        ~DummySurface() override;

        void Destroy() override;
    };
}
