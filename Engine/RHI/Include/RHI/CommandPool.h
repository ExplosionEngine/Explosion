//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_COMMAND_POOL_H
#define EXPLOSION_RHI_COMMAND_POOL_H

#include <Common/Utility.h>

namespace RHI {
    struct Queue;

    struct CommandPoolCreateInfo {
        const Queue* queue;
    };

    class CommandPool {
    public:
        NON_COPYABLE(CommandPool)
        virtual ~CommandPool();

    private:
        explicit CommandPool(const CommandPoolCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_COMMAND_POOL_H
