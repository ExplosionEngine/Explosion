//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_ENUM_H
#define EXPLOSION_RHI_ENUM_H

#include <string>

namespace RHI {
    const std::string RHI_EXT_NAME_SURFACE = "Surface";
    const std::string RHI_EXT_NAME_WINDOWS_SURFACE = "WindowsSurface";

    enum class CommandQueueType {
        GRAPHICS = 0,
        SECONDARY,
        COMPUTE,
        COPY,
        MAX
    };
}

#endif //EXPLOSION_RHI_ENUM_H
