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
    private:
        void CreateNativeCommandBuffer();

        MTLDevice& device;
    };

}
