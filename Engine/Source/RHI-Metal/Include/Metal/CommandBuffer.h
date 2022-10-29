//
// Created by Zach Lee on 2022/10/28.
//

#pragma once

#include <RHI/CommandBuffer.h>
#import <Metal/Metal.h>

namespace RHI::Metal {

    class MTLDevice;

    class MTLCommandBuffer : public CommandBuffer {
    public:
        NON_COPYABLE(MTLCommandBuffer)
        MTLCommandBuffer(MTLDevice& device);
        ~MTLCommandBuffer() override;

        void Destroy() override;

        CommandEncoder* Begin() override;

        void Reset();

        id<MTLCommandBuffer> GetNativeCommandBuffer() const;
    private:
        id<MTLCommandBuffer> CreateNativeCommandBuffer();

        MTLDevice& device;
        id<MTLCommandBuffer> commandBuffer = nil;
    };

}
