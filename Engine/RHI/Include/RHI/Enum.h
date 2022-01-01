//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_ENUM_H
#define EXPLOSION_RHI_ENUM_H

namespace RHI {
    enum class CommandQueueType {
        GRAPHICS = 0,
        SECONDARY,
        COMPUTE,
        COPY,
        MAX
    };
}

#endif //EXPLOSION_RHI_ENUM_H
