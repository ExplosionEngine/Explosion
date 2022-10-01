//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/Queue.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLQueue::MTLQueue(MTLDevice &dev) : device(dev)
    {
        CreateNativeQueue();
    }

    MTLQueue::~MTLQueue()
    {
    }

    void MTLQueue::Submit(CommandBuffer* commandBuffer, Fence* fenceToSignal)
    {

    }

    void MTLQueue::Wait(Fence* fenceToSignal)
    {

    }

    void MTLQueue::CreateNativeQueue()
    {
        id<MTLDevice> mtlDevice = device.GetDevice();
        mtlQueue = [mtlDevice newCommandQueue];
    }
}