//
// Created by johnk on 12/1/2022.
//

#ifndef EXPLOSION_RHI_GRAPHICS_CARD_H
#define EXPLOSION_RHI_GRAPHICS_CARD_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct GpuProperty {
        // TODO check different of Vulkan and DX12
        uint32_t vendorId;
        uint32_t deviceId;
        GpuType type;
        size_t memorySize;
    };

    class Gpu {
    public:
        NON_COPYABLE(Gpu)
        virtual ~Gpu();
        virtual const GpuProperty& GetProperty() = 0;

    protected:
        Gpu();
    };
}

#endif //EXPLOSION_RHI_GRAPHICS_CARD_H
