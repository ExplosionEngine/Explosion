//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#include <Metal/Queue.h>
#import <Metal/Metal.h>

#include <RHI/Queue.h>
#include <RHI/Synchronous.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLQueue : public Queue {
    public:
        NON_COPYABLE(MTLQueue)
        explicit MTLQueue(MTLDevice &dev);
        ~MTLQueue() override;

        void Submit(CommandBuffer* commandBuffer, Fence* fenceToSignal) override;
        void Wait(Fence* fenceToSignal) override;

    private:
        void CreateNativeQueue();

        MTLDevice& device;
        id<MTLCommandQueue> mtlQueue = nil;
    };
}
