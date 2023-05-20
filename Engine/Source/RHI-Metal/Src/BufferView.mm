//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/BufferView.h>
#include <Metal/Buffer.h>

namespace RHI::Metal {

    MTLBufferView::MTLBufferView(MTLBuffer &buffer, const BufferViewCreateInfo& createInfo)
        : BufferView(createInfo), mtlBuffer(buffer.GetNativeBuffer()), info(createInfo)
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

    id<MTLBuffer> MTLBufferView::GetNativeBuffer() const
    {
        return mtlBuffer;
    }

    uint32_t MTLBufferView::GetOffset() const
    {
        return info.offset;
    }

    MTLIndexType MTLBufferView::GetIndexType() const
    {
        if (info.index.format == IndexFormat::uint16) {
            return MTLIndexTypeUInt16;
        } else {
            return MTLIndexTypeUInt32;
        }
    }

}
