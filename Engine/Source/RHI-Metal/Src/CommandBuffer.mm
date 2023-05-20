//
// Created by Zach Lee on 2022/10/28.
//

#include <Metal/CommandBuffer.h>
#include <Metal/CommandEncoder.h>
#include <Metal/Device.h>
#include <Metal/Queue.h>

namespace RHI::Metal {

    MTLCommandBuffer::MTLCommandBuffer(MTLDevice& dev) : device(dev)
    {
    }

    MTLCommandBuffer::~MTLCommandBuffer()
    {
    }

    void MTLCommandBuffer::Destroy()
    {
        delete this;
    }

    CommandEncoder* MTLCommandBuffer::Begin()
    {
        return new MTLCommandEncoder(device, CreateNativeCommandBuffer());
    }

    void MTLCommandBuffer::Reset()
    {
        commandBuffer = nil;
    }

    id<MTLCommandBuffer> MTLCommandBuffer::GetNativeCommandBuffer() const
    {
        return commandBuffer;
    }

    id<MTLCommandBuffer> MTLCommandBuffer::CreateNativeCommandBuffer()
    {
        auto queue = static_cast<MTLQueue *>(device.GetQueue(QueueType::graphics, 0));
        commandBuffer = [queue->GetNativeQueue() commandBuffer];
        return commandBuffer;
    }

}
