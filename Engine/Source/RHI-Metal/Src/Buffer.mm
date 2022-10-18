//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/Buffer.h>
#include <Metal/Device.h>

namespace RHI::Metal {

    MTLBuffer::MTLBuffer(MTLDevice &device, const BufferCreateInfo* createInfo)
        : Buffer(createInfo), mtlDevice(device)
    {
        CreateNativeBuffer(createInfo);
    }

    MTLBuffer::~MTLBuffer()
    {
    }

    void* MTLBuffer::Map(MapMode mapMode, size_t offset, size_t length)
    {

    }

    void MTLBuffer::UnMap()
    {

    }

    BufferView* MTLBuffer::CreateBufferView(const BufferViewCreateInfo* createInfo)
    {

    }

    void MTLBuffer::Destroy()
    {
        delete this;
    }

    void MTLBuffer::CreateNativeBuffer(const BufferCreateInfo* createInfo)
    {
        // MTLResourceCPUCacheModeDefaultCache : Read and Write executed in expected order.
        // MTLResourceCPUCacheModeWriteCombined : CPU Write Only.
        
        // MTLResourceStorageModeShared : CPU | GPU
        // MTLResourceStorageModeManaged : CPU and GPU may maintain separate copies of the resource
        // MTLResourceStorageModePrivate : GPU
        // MTLResourceStorageModeMemoryless : Trasient

        auto &usage = createInfo->usages;
        MTLResourceOptions options = 0;
        
        if ((usage & BufferUsageBits::MAP_READ) || (usage & BufferUsageBits::COPY_SRC)) {
            options |= MTLResourceCPUCacheModeDefaultCache;
        } else {
            options |= MTLResourceCPUCacheModeWriteCombined;
        }
        
        if ((usage & BufferUsageBits::MAP_READ) || (usage & BufferUsageBits::MAP_WRITE)) {
            options |= MTLResourceStorageModeShared;
        } else {
            options |= MTLResourceStorageModePrivate;
        }
        
        mtlBuffer = [mtlDevice.GetDevice() newBufferWithLength:(createInfo->size)
                                                       options:(options)];
        
    }

}
