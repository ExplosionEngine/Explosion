//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#import <Metal/Metal.h>
#include <RHI/Buffer.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLBuffer : public Buffer {
    public:
        MTLBuffer(MTLDevice &device, const BufferCreateInfo* createInfo);
        ~MTLBuffer();

        void* Map(MapMode mapMode, size_t offset, size_t length) override;
        void UnMap() override;
        BufferView* CreateBufferView(const BufferViewCreateInfo* createInfo) override;
        void Destroy() override;

        id<MTLBuffer> GetNativeBuffer() const;
    private:
        void CreateNativeBuffer(const BufferCreateInfo* createInfo);

        MTLDevice &mtlDevice;
        id<MTLBuffer> mtlBuffer = nil;
    };

}
