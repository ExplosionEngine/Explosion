//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_COMMAND_LIST_H
#define EXPLOSION_RHI_COMMAND_LIST_H

#include <Common/Utility.h>

namespace RHI {
    class CommandBuffer;

    struct CommandBufferBeginInfo {
        const CommandBuffer* commandBuffer;
    };

    class CommandList {
    public:
        NON_COPYABLE(CommandList)
        virtual ~CommandList();

    protected:
        explicit CommandList(const CommandBufferBeginInfo* beginInfo);
    };
}

#endif //EXPLOSION_RHI_COMMAND_LIST_H
