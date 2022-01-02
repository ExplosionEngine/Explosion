//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_ENUM_H
#define EXPLOSION_RHI_ENUM_H

#include <string>

namespace RHI {
    const std::string RHI_INSTANCE_EXT_NAME_SURFACE = "EXT_INS_Surface";
    const std::string RHI_INSTANCE_EXT_NAME_WINDOWS_SURFACE = "EXT_INS_WindowsSurface";

    const std::string RHI_DEVICE_EXT_NAME_SWAP_CHAIN = "EXT_DVC_SwapChain"
}

namespace RHI {
    enum class QueueFamilyType {
        GRAPHICS = 0,
        SECONDARY,
        COMPUTE,
        COPY,
        MAX
    };
}

#endif //EXPLOSION_RHI_ENUM_H
