//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#import <Metal/Metal.h>
#include <RHI/BufferView.h>

namespace RHI::Metal {
    class MTLDevice;
    class MTLBuffer;

    class MTLBufferView : public BufferView {
    public:
        MTLBufferView(MTLBuffer &buffer, const BufferViewCreateInfo* createInfo);
        ~MTLBufferView();

        void Destroy() override;
    private:
        id<MTLBuffer> mtlBuffer = nil;
        BufferViewCreateInfo info;
    };
}
