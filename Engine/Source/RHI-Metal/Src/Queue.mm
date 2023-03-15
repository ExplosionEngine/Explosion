//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/Queue.h>
#include <Metal/Device.h>
#include <Metal/CommandBuffer.h>
#include <Metal/Synchronous.h>

namespace RHI::Metal {

    MTLQueue::MTLQueue(MTLDevice &dev) : device(dev)
    {
        CreateNativeQueue();
    }

    MTLQueue::~MTLQueue()
    {
        [mtlQueue release];
        mtlQueue = nil;
    }

    void MTLQueue::Submit(CommandBuffer* commandBuffer, Fence* fenceToSignal)
    {
        MTLCommandBuffer *mtlCommandBuffer = static_cast<MTLCommandBuffer*>(commandBuffer);
        MTLFence *mtlFence = static_cast<MTLFence*>(fenceToSignal);

        id<MTLCommandBuffer> cmd = mtlCommandBuffer->GetNativeCommandBuffer();
        mtlCommandBuffer->Reset();
        mtlFence->AddSignalEventToCmd(cmd);

        [cmd commit];
    }

    void MTLQueue::Wait(Fence* fenceToSignal)
    {

    }

    id<MTLCommandQueue> MTLQueue::GetNativeQueue() const
    {
        return mtlQueue;
    }

    void MTLQueue::CreateNativeQueue()
    {
        id<MTLDevice> mtlDevice = device.GetDevice();
        mtlQueue = [mtlDevice newCommandQueue];
    }
}
