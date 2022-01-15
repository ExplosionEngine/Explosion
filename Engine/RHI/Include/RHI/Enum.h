//
// Created by johnk on 10/1/2022.
//

#ifndef EXPLOSION_RHI_ENUM_H
#define EXPLOSION_RHI_ENUM_H

namespace RHI {
    enum class RHIType {
        DIRECTX_12,
        VULKAN,
        METAL,
        GNN,
        MAX
    };

    enum class GpuType {
        HARDWARE,
        SOFTWARE,
        MAX
    };

    enum class QueueType {
        GRAPHICS,
        COMPUTE,
        TRANSFER,
        MAX
    };
}

#endif //EXPLOSION_RHI_ENUM_H
