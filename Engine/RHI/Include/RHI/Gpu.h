//
// Created by johnk on 12/1/2022.
//

#ifndef EXPLOSION_RHI_GRAPHICS_CARD_H
#define EXPLOSION_RHI_GRAPHICS_CARD_H

#include <cstdint>

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct GpuProperty {
        uint32_t vendorId;
        uint32_t deviceId;
        GpuType type;
    };

    class Gpu {
    public:
        NON_COPYABLE(Gpu)
        virtual ~Gpu();
        virtual GpuProperty GetProperty() = 0;

    protected:
        Gpu();
    };
}

#endif //EXPLOSION_RHI_GRAPHICS_CARD_H
