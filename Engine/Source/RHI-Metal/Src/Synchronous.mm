//
// Created by Zach Lee on 2022/10/28.
//

#include <Metal/Synchronous.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLFence::MTLFence(MTLDevice& dev) : Fence(dev), device(dev)
    {
        sharedEvent = [device.GetDevice() newSharedEvent];
    }

    MTLFence::~MTLFence()
    {
        [sharedEvent release];
        sharedEvent = nil;
    }

    FenceStatus MTLFence::GetStatus()
    {
        return FenceStatus::SIGNALED;
    }

    void MTLFence::Reset()
    {
        sharedEvent.signaledValue = 0u;
    }

    void MTLFence::Wait()
    {
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        [sharedEvent notifyListener:device.GetSharedEventListener()
                             atValue:1 block:^(id<MTLSharedEvent> sharedEvent, uint64_t value)
                                 {
                                      dispatch_semaphore_signal(semaphore);
                                 }];
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        dispatch_release(semaphore);
    }

    void MTLFence::Destroy()
    {
        delete this;
    }

    void MTLFence::AddSignalEventToCmd(id<MTLCommandBuffer> commandBuffer)
    {
        [commandBuffer encodeSignalEvent:sharedEvent value:1];
    }
}
