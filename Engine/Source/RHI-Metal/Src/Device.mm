//
// Created by Zach Lee on 2022/9/22.
//

#include <Metal/Device.h>

namespace RHI::Metal {
    MTLDevice::MTLDevice(MTLGpu& gpu_, const DeviceCreateInfo* createInfo) : Device(createInfo), gpu(gpu_)
    {
    }

    MTLDevice::~MTLDevice()
    {
    }

    size_t MTLDevice::GetQueueNum(QueueType type)
    {
        return 0;
    }
//    Queue* MTLDevice::GetQueue(QueueType type, size_t index)
//    SwapChain* MTLDevice::CreateSwapChain(const SwapChainCreateInfo* createInfo)
//    Buffer* MTLDevice::CreateBuffer(const BufferCreateInfo* createInfo)
//    Texture* MTLDevice::CreateTexture(const TextureCreateInfo* createInfo)
//    Sampler* MTLDevice::CreateSampler(const SamplerCreateInfo* createInfo)
//    BindGroupLayout* MTLDevice::CreateBindGroupLayout(const BindGroupLayoutCreateInfo* createInfo)
//    BindGroup* MTLDevice::CreateBindGroup(const BindGroupCreateInfo* createInfo)
//    PipelineLayout* CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo)
//    ShaderModule* MTLDevice::CreateShaderModule(const ShaderModuleCreateInfo* createInfo)
//    ComputePipeline* MTLDevice::CreateComputePipeline(const ComputePipelineCreateInfo* createInfo)
//    GraphicsPipeline* MTLDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo)
//    CommandBuffer* MTLDevice::CreateCommandBuffer()
//    Fence* MTLDevice::CreateFence()

    void MTLDevice::Destroy()
    {
        delete this;
    }

    MTLGpu& MTLDevice::GetGpu() const
    {
        return gpu;
    }

}