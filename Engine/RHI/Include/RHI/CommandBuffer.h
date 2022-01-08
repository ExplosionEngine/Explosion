//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_COMMAND_BUFFER_H
#define EXPLOSION_RHI_COMMAND_BUFFER_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    class CommandPool;

    struct CommandBufferAllocateInfo {
        const CommandPool* commandPool;
        CommandBufferLevel level;
        size_t num;
    };

    class CommandBuffer {
    public:
        NON_COPYABLE(CommandBuffer)
        virtual ~CommandBuffer();

    private:
        explicit CommandBuffer(const CommandBufferAllocateInfo* allocateInfo);
    };
}

#endif //EXPLOSION_RHI_COMMAND_BUFFER_H
