//
// Created by Zach Lee on 2022/10/28.
//

#pragma once

#include <RHI/Synchronous.h>
#import <Metal/Metal.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLFence : public Fence {
    public:
        explicit MTLFence(MTLDevice& device);
        ~MTLFence();

        FenceStatus GetStatus() override;
        void Reset() override;
        void Wait() override;
        void Destroy() override;

        void AddSignalEventToCmd(id<MTLCommandBuffer> commandBuffer);

    private:
        MTLDevice& device;
        id<MTLSharedEvent> sharedEvent = nil;
    };
}
