//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <Common/Utility.h>

namespace RHI {
    class CommandBuffer;

    class Queue {
    public:
        NON_COPYABLE(Queue)
        virtual ~Queue();

        virtual void Submit(CommandBuffer* commandBuffer) = 0;

    protected:
        Queue();
    };
}
