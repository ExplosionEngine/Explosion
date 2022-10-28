//
// Created by Zach Lee on 2022/9/22.
//

#include <Metal/Device.h>
#include <Metal/Gpu.h>
#include <Metal/ShaderModule.h>
#include <Metal/SwapChain.h>
#include <Metal/Texture.h>
#include <Metal/TextureView.h>
#include <Metal/Buffer.h>
#include <Metal/BufferView.h>
#include <Metal/Pipeline.h>
#include <Metal/PipelineLayout.h>

namespace RHI::Metal {
    MTLDevice::MTLDevice(MTLGpu& gpu_, const DeviceCreateInfo* createInfo) : Device(createInfo), gpu(gpu_)
    {
        mtlDevice = gpu.GetDevice();
    }

    MTLDevice::~MTLDevice()
    {
    }

    size_t MTLDevice::GetQueueNum(QueueType type)
    {
        return 1;
    }

    Queue* MTLDevice::GetQueue(QueueType type, size_t index)
    {
        auto &queueList = queues[type];
        if (index >= queueList.size()) {
            return nullptr;
        }
        if (queueList.empty()) {
            queueList.emplace_back(new MTLQueue(*this));
        }
        return queueList[index].get();
    }

    SwapChain* MTLDevice::CreateSwapChain(const SwapChainCreateInfo* createInfo)
    {
        return new MTLSwapChain(*this, createInfo);
    }

    Buffer* MTLDevice::CreateBuffer(const BufferCreateInfo* createInfo)
    {
        return new MTLBuffer(*this, createInfo);
    }

    Texture* MTLDevice::CreateTexture(const TextureCreateInfo* createInfo)
    {
        return new MTLTexture(*this, createInfo);
    }

    PipelineLayout* MTLDevice::CreatePipelineLayout(const PipelineLayoutCreateInfo* createInfo)
    {
        return new MTLPipelineLayout(*this, createInfo);
    }

    ShaderModule* MTLDevice::CreateShaderModule(const ShaderModuleCreateInfo* createInfo)
    {
        return new MTLShaderModule(*this, createInfo);
    }

    GraphicsPipeline* MTLDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo* createInfo)
    {
        return new MTLGraphicsPipeline(*this, createInfo);
    }

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

    id<MTLDevice> MTLDevice::GetDevice() const
    {
        return mtlDevice;
    }
}
