//
// Created by johnk on 15/1/2022.
//

#pragma once

#include <Common/Utility.h>

namespace RHI {
    class CommandBuffer;
    class Fence;

    class Queue {
    public:
        NON_COPYABLE(Queue)
        virtual ~Queue();

        virtual void Submit(CommandBuffer* commandBuffer, Fence* fenceToSignal) = 0;
        virtual void Wait(Fence* fenceToSignal) = 0;

    protected:
        Queue();
    };
}
