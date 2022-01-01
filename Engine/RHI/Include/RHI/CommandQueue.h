//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_COMMAND_QUEUE_H
#define EXPLOSION_RHI_COMMAND_QUEUE_H

#include <RHI/Enum.h>
#include <Common/Utility.h>

namespace RHI {
    struct CommandQueueCreateInfo {
        CommandQueueType type;
    };

    class CommandQueue {
    public:
        NON_COPYABLE(CommandQueue)
        virtual ~CommandQueue();

    protected:
        CommandQueue();
    };
}

#endif //EXPLOSION_RHI_COMMAND_QUEUE_H
