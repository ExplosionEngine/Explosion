//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/BufferView.h>
#include <Metal/Buffer.h>

namespace RHI::Metal {

    MTLBufferView::MTLBufferView(MTLBuffer &buffer, const BufferViewCreateInfo* createInfo)
        : BufferView(createInfo), mtlBuffer(buffer.GetNativeBuffer()), info(*createInfo)
    {
        [mtlBuffer retain];
    }

    MTLBufferView::~MTLBufferView()
    {
        [mtlBuffer release];
    }

    void MTLBufferView::Destroy()
    {
        delete this;
    }
}
