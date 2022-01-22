//
// Created by johnk on 10/1/2022.
//

#ifndef EXPLOSION_RHI_ENUM_H
#define EXPLOSION_RHI_ENUM_H

namespace RHI {
    using EnumType = uint32_t;

    enum class RHIType : EnumType {
        DIRECTX_12,
        VULKAN,
        METAL,
        GNN,
        MAX
    };

    enum class GpuType : EnumType {
        HARDWARE,
        SOFTWARE,
        MAX
    };

    enum class QueueType : EnumType {
        GRAPHICS,
        COMPUTE,
        TRANSFER,
        MAX
    };
}

namespace RHI {
    using FlagType = uint32_t;
}

#endif //EXPLOSION_RHI_ENUM_H
